#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <ctype.h>

#define MYREAD 0
#define MYWRITE 1

#define ERR_EXIT(a) do { perror(a); exit(1); } while(0)

#define OBJ_NUM 3
#define OFFSET 902001
#define FOOD_INDEX 0
#define CONCERT_INDEX 1
#define ELECS_INDEX 2
#define RECORD_PATH "./bookingRecord"

static char* obj_names[OBJ_NUM] = {"Food", "Concert", "Electronics"};

typedef struct {
    char hostname[512];  // server's hostname
    unsigned short port;  // port to listen
    int listen_fd;  // fd to wait for a new connection
} server;

typedef struct {
    char host[512];  // client's host
    int conn_fd;  // fd to talk with client
    char buf[512];  // data sent by/to client
    size_t buf_len;  // bytes used by buf
    int id;
    int wait_for_write;  // used by handle_read to know if the header is read or not.
} request;

server svr;  // server
request* requestP = NULL;  // point to a list of requests
int maxfd;  // size of open file descriptor table, size of request list

const char* accept_read_header = "ACCEPT_FROM_READ";
const char* accept_write_header = "ACCEPT_FROM_WRITE";
const unsigned char IAC_IP[3] = "\xff\xf4";

static void init_server(unsigned short port);
// initailize a server, exit for error

static void init_request(request* reqP);
// initailize a request instance

static void free_request(request* reqP);
// free resources used by a request instance

typedef struct {
    int id;          // 902001-902020
    int bookingState[OBJ_NUM]; // 1 means booked, 0 means not.
}record;

int handle_read(request* reqP) {
    /*  Return value:
     *      1: read successfully
     *      0: read EOF (client down)
     *     -1: read failed
     */
    int r;
    char buf[512] = "";

    // Read in request from client
    r = read(reqP->conn_fd, buf, sizeof(buf));
    if (r < 0) return -1;
    if (r == 0) return 0;
    char* p1 = strstr(buf, "\015\012");
    int newline_len = 2;
    if (p1 == NULL) {
       p1 = strstr(buf, "\012");
        if (p1 == NULL) {
            if (!strncmp(buf, IAC_IP, 2)) {
                // Client presses ctrl+C, regard as disconnection
                fprintf(stderr, "Client presses ctrl+C....\n");
                return 0;
            }
            ERR_EXIT("this really should not happen...");
        }
    }
    size_t len = p1 - buf + 1;
    memmove(reqP->buf, buf, len);
    reqP->buf[len - 1] = '\0';
    reqP->buf_len = len-1;
    return 1;
}

int nums[3] = {0, 0, 0};
char globalbookid[512];
int conn_fd;  // fd for a new connection with client
int file_fd;  // fd for file that we open for reading
char buf[512];
int buf_len;
char lockedheader[] = "Locked.\n";
char header[] = "Please enter your id (to check your booking state):\n";
char end[] = "(Type Exit to leave...)\n";
char header2[] = "Please input your booking command. (Food, Concert, Electronics. Positive/negative value increases/decreases the booking amount.):\n";
char errorheader0[] = "[Error] Operation failed. Please try again.\n";
char errorheader[] = "[Error] Sorry, but you cannot book more than 15 items in total.\n";
char errorheader2[] = "[Error] Sorry, but you cannot book less than 0 items.\n";

int idused[20] = {0};

bool validoperation(char *str, int len){
    if (len > 10) return false;
    int id = atoi(str);
    if (id < 902001 || id > 902020) return false;
    for (int i = 0; i < len; i++){
        if (isalpha(str[i])) return false;
    }
    return true;
}

bool validbookingcommand(char *str, int len){
    char delim[] = " ";
    int parts = 0;
    char *ptr = strtok(str, delim);
    while (ptr != NULL){
        for (int i = 0; i < strlen(ptr); i++){
            if (ptr[i] == '-'){
                if (i < strlen(ptr) - 1){
                    if (isdigit(ptr[i+1]) == false){
                        return false;
                    }
                }
                
                else if (i == strlen(ptr) - 1){
                    return false;
                }
            }
            
            else if (isalpha(ptr[i]))return false;
        }
        parts++;
        printf("parts = %s\n", ptr);
        ptr = strtok(NULL, delim);
    }
    printf("%d\n", parts);
    if (parts != 3) return false;
    return true;
}

void closefd(int fd, fd_set* master, request* reqP, int readorwrite){
    //printf("bookid = %d\n", atoi(globalbookid));
    printf("in\n");
    printf("operation = %s\n", globalbookid);
    if (validoperation(globalbookid, strlen(globalbookid))){
        printf("in2\n");
        if (readorwrite == MYREAD){
            printf("in3\n");
            printf("idused = %d\n", idused[atoi(globalbookid)-OFFSET]);
            if (idused[atoi(globalbookid)-OFFSET] > 0)
                printf("readid = 0\n");
                idused[atoi(globalbookid)-OFFSET]--;

            //unlock the region if within the same process, all of the read request are done
            if (idused[atoi(globalbookid)-OFFSET] == 0){
                struct flock lock;
                lock.l_type = F_UNLCK;
                lock.l_whence = SEEK_SET;
                lock.l_start = sizeof(record)*(atoi(globalbookid)-OFFSET);
                lock.l_len = sizeof(record);
                fcntl(file_fd, F_SETLK, &lock);
                printf("unlocked read lock\n");
            }
        }
        else{
            
            if (idused[atoi(globalbookid)-OFFSET] < 0)
                idused[atoi(globalbookid)-OFFSET]++;

            //unlock the write lock if within the same region, the write request is done
            struct flock lock;
            lock.l_type = F_UNLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = sizeof(record)*(atoi(globalbookid)-OFFSET);
            lock.l_len = sizeof(record);
            fcntl(file_fd, F_SETLK, &lock);
        }
    }
    FD_CLR(fd, master);
    close(fd);
    free_request(reqP);
}

void myreadserver(request* reqP, fd_set *master){
    if (reqP->wait_for_write == 0){
        if (validoperation(reqP->buf, reqP->buf_len)){

            record input;
            int bookid = atoi(reqP->buf);
            strcpy(globalbookid, reqP->buf);
            //for the same process, increase the num if there is one more client who wants to read
            idused[bookid-OFFSET]++;

            struct flock lock;
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = sizeof(record)*(bookid-OFFSET);
            lock.l_len = sizeof(record);
            
            //if the current book id has no lock or more than zero read locks, that means we can read
            if (idused[bookid-OFFSET] >= 0 && fcntl(file_fd, F_SETLK, &lock) != -1){
                lseek(file_fd, sizeof(record)*(bookid-902001), SEEK_SET);
                read(file_fd, &input, sizeof(record));
                for (int i = 0; i < 3; i++){
                    sprintf(buf, "%s: %d booked\n", obj_names[i], input.bookingState[i]);
                    write(reqP->conn_fd, buf, strlen(buf));
                }

                reqP->wait_for_write++;
                write(reqP->conn_fd, "\n", sizeof("\n"));
            }
            //else, that means the current book id has write locks from different process
            else{
                write(reqP->conn_fd, lockedheader, sizeof(lockedheader));
                closefd(reqP->conn_fd, master, reqP, MYREAD);
            }
        }
        else{    
            write(reqP->conn_fd, errorheader0, sizeof(errorheader0));
            closefd(reqP->conn_fd, master, reqP, MYREAD);
            printf("line201\n");
        }
    }

    if (reqP->wait_for_write == 1){
        write(reqP->conn_fd, end, sizeof(end));
        reqP->wait_for_write++;
    }

    else if (reqP->wait_for_write == 2){
        printf("input=%s\n", reqP->buf);
        if (strcmp(reqP->buf, "Exit") == 0){
            closefd(reqP->conn_fd, master, reqP, MYREAD);
        }
    }
}

void mywriteserver(request* reqP, fd_set *master){
    if (reqP->wait_for_write == 0){
        if (validoperation(reqP->buf, reqP->buf_len)){
            record input;
            int bookid = atoi(reqP->buf);
            strcpy(globalbookid, reqP->buf);

            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = sizeof(record)*(bookid-902001);
            lock.l_len = sizeof(record);

            //if fcntl return -1, that means giving write lock fails
            //that means the region already has read lock or write locks
            //if fcntl != -1, that means we can give write lock
            if (idused[bookid-OFFSET] == 0 && fcntl(file_fd, F_SETLK, &lock) != -1){
                
                if (idused[bookid-OFFSET] != 0){
                    write(reqP->conn_fd, lockedheader, sizeof(lockedheader));
                    closefd(reqP->conn_fd, master, reqP, MYWRITE);
                }
                idused[bookid-OFFSET] = -1;

                //strcpy(globalbookid, reqP->buf);
                lseek(file_fd, sizeof(record)*(bookid-902001), SEEK_SET);
                read(file_fd, &input, sizeof(record));
                for (int i = 0; i < 3; i++){
                    sprintf(buf, "%s: %d booked\n", obj_names[i], input.bookingState[i]);
                    write(reqP->conn_fd, buf, strlen(buf));
                }
                reqP->wait_for_write++;
                write(reqP->conn_fd, "\n", sizeof("\n"));
            }
            else{
                write(reqP->conn_fd, lockedheader, sizeof(lockedheader));
                closefd(reqP->conn_fd, master, reqP, MYWRITE);
            }      
        }
        else{
            write(reqP->conn_fd, errorheader0, sizeof(errorheader0));
            closefd(reqP->conn_fd, master, reqP, MYWRITE);
        }
    }
    
    if (reqP->wait_for_write == 1){
        write(reqP->conn_fd, header2, sizeof(header2));
        reqP->wait_for_write++;
    }
    
    else if (reqP->wait_for_write == 2){
        char delim[] = " ";
        strcpy(buf, reqP->buf);
        printf("input = %s\n", reqP->buf);
        if (validbookingcommand(reqP->buf, reqP->buf_len)){
            printf("input is valid\n");
            char *ptr = strtok(buf, delim);
            int index = 0;
            while (ptr != NULL){
                nums[index] = atoi(ptr);
                ptr = strtok(NULL, delim);
                index++;
            }
            reqP->wait_for_write++;
        }
        else{
            write(reqP->conn_fd, errorheader0, sizeof(errorheader0));
            closefd(reqP->conn_fd, master, reqP, MYWRITE);
        }
    }

    if (reqP->wait_for_write == 3){
        if (validoperation(globalbookid, strlen(globalbookid))){
            int bookid = atoi(globalbookid);
            lseek(file_fd, sizeof(record)*(bookid-902001), SEEK_SET);
            record newinput;
            read(file_fd, &newinput, sizeof(record));
            int totalitems = 0;
            bool lowerboundflag = true;
            for (int i = 0; i < 3; i++){
                if (newinput.bookingState[i] + nums[i] < 0){
                    lowerboundflag = false;
                }
                newinput.bookingState[i] += nums[i];
                totalitems += newinput.bookingState[i];
            }

            if (totalitems > 15){
                write(reqP->conn_fd, errorheader, sizeof(errorheader));
                closefd(reqP->conn_fd, master, reqP, MYWRITE);
            }
            else if (lowerboundflag == false){
                write(reqP->conn_fd, errorheader2, sizeof(errorheader2));
                closefd(reqP->conn_fd, master, reqP, MYWRITE);
            }
            else{
                lseek(file_fd, sizeof(record)*(bookid-902001), SEEK_SET);
                write(file_fd, &newinput, sizeof(newinput));
                char header3[512];
                sprintf(header3, "Bookings for user %d are updated, the new booking state is:\n", bookid);
                write(reqP->conn_fd, header3, strlen(header3));
                for (int i = 0; i < 3; i++){
                    sprintf(buf, "%s: %d booked\n", obj_names[i], newinput.bookingState[i]);
                    write(reqP->conn_fd, buf, strlen(buf));
                }
            }
            nums[0] = 0, nums[1] = 0, nums[2] = 0;
            closefd(reqP->conn_fd, master, reqP, MYWRITE);
        }
        else{
            write(reqP->conn_fd, errorheader0, strlen(errorheader0));
            closefd(reqP->conn_fd, master, reqP, MYWRITE);
        }
    }

}

int main(int argc, char** argv) {

    // Parse args.
    if (argc != 2) {
        fprintf(stderr, "usage: %s [port]\n", argv[0]);
        exit(1);
    }

    struct sockaddr_in cliaddr;  // used by accept()
    int clilen;

    // Initialize server
    init_server((unsigned short) atoi(argv[1]));

    // Loop for handling connections
    fprintf(stderr, "\nstarting on %.80s, port %d, fd %d, maxconn %d...\n", svr.hostname, svr.port, svr.listen_fd, maxfd);

    file_fd = open(RECORD_PATH, O_RDWR);
    fd_set readmaster;
    fd_set readfds;
    FD_ZERO(&readmaster);
    FD_ZERO(&readfds);
    FD_SET(svr.listen_fd, &readmaster);
    fcntl(svr.listen_fd, F_SETFL, O_NONBLOCK);
    

    while (1) {
        // TODO: Add IO multiplexing
        memcpy(&readfds, &readmaster, sizeof(readmaster));
        if (select(maxfd+1, &readfds, NULL, NULL, NULL) <= 0){
            continue;
        }

        if (FD_ISSET(svr.listen_fd, &readfds)){
        // Check new connection
            clilen = sizeof(cliaddr);
            conn_fd = accept(svr.listen_fd, (struct sockaddr*)&cliaddr, (socklen_t*)&clilen);
            if (conn_fd < 0) {
                if (errno == EINTR || errno == EAGAIN) continue;  // try again
                if (errno == ENFILE) {
                    (void) fprintf(stderr, "out of file descriptor table ... (maxconn %d)\n", maxfd);
                    continue;
                }
                ERR_EXIT("accept");
            }
            requestP[conn_fd].conn_fd = conn_fd;
            strcpy(requestP[conn_fd].host, inet_ntoa(cliaddr.sin_addr));
            fprintf(stderr, "getting a new request... fd %d from %s\n", conn_fd, requestP[conn_fd].host);
            FD_SET(conn_fd, &readmaster);
            write(conn_fd, header, sizeof(header));
            requestP[conn_fd].wait_for_write = 0;
        }

        else{
            conn_fd = -1;
            for (int i = 0; i <= maxfd; i++){
                if (i != svr.listen_fd && FD_ISSET(i, &readfds)){
                    conn_fd = i;
                    int ret = handle_read(&requestP[conn_fd]); // parse data from client to requestP[conn_fd].buf
                    fprintf(stderr, "ret = %d\n", ret);
	                if (ret < 0) {
                        fprintf(stderr, "bad request from %s\n", requestP[conn_fd].host);
                        continue;
                    }
#ifdef READ_SERVER  
                    if (ret <= 0){
                        closefd(requestP[conn_fd].conn_fd, &readmaster, &requestP[conn_fd], MYREAD);
                    }
                    else{
                        myreadserver(&requestP[conn_fd], &readmaster);
                    }
                    
#elif defined WRITE_SERVER
                    if (ret <= 0){
                        closefd(requestP[conn_fd].conn_fd, &readmaster, &requestP[conn_fd], MYWRITE);
                    }
                    else{
                        mywriteserver(&requestP[conn_fd], &readmaster);
                    }
                    
#endif
                }
            }
        }
    }
    free(requestP);
    return 0;
}

// ======================================================================================================
// You don't need to know how the following codes are working
#include <fcntl.h>

static void init_request(request* reqP) {
    reqP->conn_fd = -1;
    reqP->buf_len = 0;
    reqP->id = 0;
}

static void free_request(request* reqP) {
    /*if (reqP->filename != NULL) {
        free(reqP->filename);
        reqP->filename = NULL;
    }*/
    init_request(reqP);
}

static void init_server(unsigned short port) {
    struct sockaddr_in servaddr;
    int tmp;

    gethostname(svr.hostname, sizeof(svr.hostname));
    svr.port = port;

    svr.listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (svr.listen_fd < 0) ERR_EXIT("socket");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    tmp = 1;
    if (setsockopt(svr.listen_fd, SOL_SOCKET, SO_REUSEADDR, (void*)&tmp, sizeof(tmp)) < 0) {
        ERR_EXIT("setsockopt");
    }
    if (bind(svr.listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        ERR_EXIT("bind");
    }
    if (listen(svr.listen_fd, 1024) < 0) {
        ERR_EXIT("listen");
    }

    // Get file descripter table size and initialize request table
    maxfd = getdtablesize();
    requestP = (request*) malloc(sizeof(request) * maxfd);
    if (requestP == NULL) {
        ERR_EXIT("out of memory allocating all requests");
    }
    for (int i = 0; i < maxfd; i++) {
        init_request(&requestP[i]);
    }
    requestP[svr.listen_fd].conn_fd = svr.listen_fd;
    strcpy(requestP[svr.listen_fd].host, svr.hostname);

    return;
}