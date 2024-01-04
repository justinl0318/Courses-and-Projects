#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_DEPTH 20

char prefix[6][11]; //array of string to store ancestor's (parent) name

void print(char *basename, int level, int is_last[], char keyword) {
    int count = 0;
    for (int i = 0; i <= level; i++){
        int len = strlen(prefix[i]);
        for (int j = 0; j < len; j++){
            if (prefix[i][j] == keyword) count++;
        }
        printf("%s", prefix[i]);
        if (i != level) printf("/");
    }
    
    printf(" %d\n", count);

}

void traverse(char *path, char *basename, int level, int is_last[], int *file_num,
              int *dir_num, char keyword) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        printf("%s [error opening dir]\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "tree: cannot stat (new recursion) %s\n", path);
        return;
    } else {
        close(fd);
    }

    if (st.type == T_FILE) {
        if (level == 0) {
            printf("%s [error opening dir]\n", path);
            return;
        }
        (*file_num)++;
        print(basename, level, is_last, keyword);
        return;
    } else if (st.type == T_DIR) {
        if (level > 0) {
            (*dir_num)++;
        }
        print(basename, level, is_last, keyword);
    } else {
        // printf("tree: path %s is a device\n", path);
        return;
    }

    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
        printf("tree: path too long\n");
        close(fd);
        return;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    int count = 0;
    if ((fd = open(path, 0)) < 0) {
        printf("%s [error opening dir]\n", path);
        return;
    }
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)
            continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if (strcmp(de.name, ".") && strcmp(de.name, "..")) {
            count++;
        }
    }
    close(fd);
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "tree: cannot open %s after counting files\n", path);
        return;
    }
    // read files under current path
    int cnt = 0;
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)
            continue;

        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if (strcmp(de.name, ".") && strcmp(de.name, "..")) {
            if (cnt == count - 1) {
                is_last[level] = 1;
            }
            cnt++;
            strcpy(prefix[level+1], de.name);
            traverse(buf, de.name, level + 1, is_last, file_num, dir_num, keyword);
            strcpy(prefix[level+1], "");
            is_last[level] = 0;
        }
    }

    close(fd);
    return;
}

int main(int argc, char *argv[]) {
    // printf("stdout\n");
    // fprintf(2, "stderr\n");
    // int ret = 0;
    int pid, ret = 0;
    int fds[2];

    if (argc < 2) {
        printf("tree: missing argv[1]\n");
        exit(-1);
    }

    // Create pipes
    if (pipe(fds) < 0) {
        printf("tree: pipe failed\n");
        exit(-1);
    }

    // Create child process
    pid = fork();
    if (pid == 0) { // Child
        int file_num = 0, dir_num = 0;
        int is_last[MAX_DEPTH] = {};
        strcpy(prefix[0], argv[1]);
        traverse(argv[1], argv[1], 0, is_last, &file_num, &dir_num, argv[2][0]);

        write(fds[1], &file_num, sizeof(int));
        write(fds[1], &dir_num, sizeof(int));
        printf("\n");

        exit(0);
    } else if (pid > 0) { // Parent
        // wait for child to exit
        // wait(0);
        int file_num, dir_num;
        if (read(fds[0], &file_num, sizeof(int)) != sizeof(int)) {
            printf("tree: pipe read failed (file_num)\n");
        }
        if (read(fds[0], &dir_num, sizeof(int)) != sizeof(int)) {
            printf("tree: pipe read failed (dir_num)\n");
        }
        printf("%d directories, %d files\n", dir_num, file_num);
    } else {
        printf("tree: fork failed\n");
        ret = -1;
    }

    close(fds[0]);
    close(fds[1]);
    exit(ret);
}
