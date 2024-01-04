#include "header.h"

#define MYDEPTH 3
#define PRFLAG (PROT_READ | PROT_WRITE)
#define MFLAG (MAP_SHARED | MAP_ANONYMOUS)

typedef struct movienscorearr{
	char **titles; //array of strings
	double *scores; //array of scores
}movienscorearr;

typedef struct movienscorensize{
	int id;
	int size;
	int left;
	int right;
	int depth;
	int index;
}movienscorensize;

#ifdef PROCESS

#elif defined THREAD
movienscorearr *msarrs[MAX_REQ]; //array of pointers that points to movienscorearr
#endif

movie_profile* movies[MAX_MOVIES];
unsigned int num_of_movies = 0;
unsigned int num_of_reqs = 0;

// Global request queue and pointer to front of queue
// TODO: critical section to protect the global resources
request* reqs[MAX_REQ];
int front = -1;

/* Note that the maximum number of processes per workstation user is 512. * 
 * We recommend that using about <256 threads is enough in this homework. */
pthread_t tid[MAX_CPU][MAX_THREAD]; //tids for multithread

#ifdef PROCESS
pid_t pid[MAX_CPU][MAX_THREAD]; //pids for multiprocess
#endif

//mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void initialize(FILE* fp);
request* read_request();
int pop();

int pop(){
	front+=1;
	return front;
}

double calscore(request *req, movie_profile *mov){
	double score = 0;
	for (int i = 0; i < NUM_OF_GENRE; i++){
		score += (req->profile[i] * mov->profile[i]);
	}
	return score;
}

#ifdef THREAD
void merge(void *arg){
	
	movienscorensize *ptrs = (movienscorensize *) arg;
	int mid = (ptrs->left + ptrs->right) / 2;
	int lsize = mid - ptrs->left + 1, rsize = ptrs->right - mid;
	char **larrtitle = (char **) malloc(sizeof(char *) * lsize);
	double *larrscore = (double *) malloc(sizeof(double) * lsize);
	for (int i = 0; i < lsize; i++){
		char *s = msarrs[ptrs->index]->titles[ptrs->left+i];
		larrtitle[i] = s;
		larrscore[i] = msarrs[ptrs->index]->scores[ptrs->left+i];
	}

	char **rarrtitle = (char **) malloc(sizeof(char *) * rsize);
	double *rarrscore = (double *) malloc(sizeof(double) * rsize);
	for (int i = 0; i < rsize; i++){
		char *s = msarrs[ptrs->index]->titles[mid+1+i];
		rarrtitle[i] = s;
		rarrscore[i] = msarrs[ptrs->index]->scores[mid+1+i];
	}
	
	int k = ptrs->left;
	int i = 0, j = 0;
	while (i < lsize && j < rsize){
		if (larrscore[i] > rarrscore[j] || (larrscore[i] == rarrscore[j] && strcmp(larrtitle[i], rarrtitle[j]) < 0)){
			msarrs[ptrs->index]->titles[k] = larrtitle[i];
			msarrs[ptrs->index]->scores[k] = larrscore[i];
			k++, i++;
		}
		else{
			msarrs[ptrs->index]->titles[k] = rarrtitle[j];
			msarrs[ptrs->index]->scores[k] = rarrscore[j];
			k++, j++;
		}
	}

	while (i < lsize){
		msarrs[ptrs->index]->titles[k] = larrtitle[i];
		msarrs[ptrs->index]->scores[k] = larrscore[i];
		k++, i++;
	}
	while (j < rsize){
		msarrs[ptrs->index]->titles[k] = rarrtitle[j];
		msarrs[ptrs->index]->scores[k] = rarrscore[j];
		k++, j++;
	}
	free(larrtitle), free(larrscore);
	free(rarrtitle), free(rarrscore);
}

void *merge_sort(void *arg){
	movienscorensize *ptrs = (movienscorensize *) arg;
	
	int left = ptrs->left;
	int right = ptrs->right;
	int mid = (left + right) / 2;
	int depth = ptrs->depth;
	//printf("left = %d, right = %d, depth = %d\n", left, right, depth);
	if (depth < MYDEPTH && left < right){
		pthread_t threadl;
		movienscorensize *ptrsl = (movienscorensize *) malloc(sizeof(movienscorensize));
		memcpy(ptrsl, ptrs, sizeof(movienscorensize));
		ptrsl->left = left, ptrsl->right = mid;
		ptrsl->depth++;

		pthread_t threadr;
		movienscorensize *ptrsr = (movienscorensize *) malloc(sizeof(movienscorensize));
		memcpy(ptrsr, ptrs, sizeof(movienscorensize));
		ptrsr->left = mid+1, ptrs->right = right;
		ptrsr->depth++;

		pthread_create(&threadl, NULL, merge_sort, ptrsl);
		pthread_create(&threadr, NULL, merge_sort, ptrsr);

		pthread_join(threadr, NULL);
		pthread_join(threadl, NULL);

		merge(ptrs);
	}
	else{
		//using sort() library
		int dsize = right - left + 1;
		if (dsize == 1){
			sort(msarrs[ptrs->index]->titles + left, msarrs[ptrs->index]->scores + left, dsize);
		}
		else{
			int lsize = mid - left + 1;
			int rsize = right - mid;
			sort(msarrs[ptrs->index]->titles + left, msarrs[ptrs->index]->scores + left, lsize);
			sort(msarrs[ptrs->index]->titles + mid + 1, msarrs[ptrs->index]->scores + mid + 1, rsize);
			merge(ptrs);
		}
	}
}

void *thread_start(void *arg){
	
	pthread_mutex_lock(&lock);	//should add mutex here to lock front
	int fr = pop();
	pthread_mutex_unlock(&lock); //release mutex here
	
	movienscorensize *ptrs = (movienscorensize *) malloc(sizeof(movienscorensize));
	ptrs->size = 0;

	movienscorearr *msarr = (movienscorearr *) malloc(sizeof(movienscorearr));
	msarr->titles = (char **) malloc(sizeof(char *) * num_of_movies);
	msarr->scores = (double *) malloc(sizeof(double) * num_of_movies);
	bool all = false;
	if (strcmp(reqs[fr]->keywords, "*") == 0) all = true;
	for (int j = 0; j < num_of_movies; j++){
		if (all){
			char *s = (char *) malloc(sizeof(char) * MAX_LEN);
			strcpy(s, movies[j]->title);
			double score = calscore(reqs[fr], movies[j]);
			msarr->titles[ptrs->size] = s;
			msarr->scores[ptrs->size] = score;
			ptrs->size++;
		}
		else{
			if (strstr(movies[j]->title, reqs[fr]->keywords) != NULL){
				char *s = (char *) malloc(sizeof(char) * MAX_LEN);
				strcpy(s, movies[j]->title);
				double score = calscore(reqs[fr], movies[j]);
				msarr->titles[ptrs->size] = s;
				msarr->scores[ptrs->size] = score;
				ptrs->size++;
			}
		}
	}

	//printf("fr = %d, size = %d, keyword = %s\n", fr, ptrs->size, reqs[fr]->keywords);
	ptrs->index = fr;
	msarrs[ptrs->index] = msarr;	
	ptrs->left = 0, ptrs->right = ptrs->size - 1;
	ptrs->id = reqs[fr]->id;
	
	pthread_create(&tid[ptrs->index][0], NULL, merge_sort, ptrs);
	pthread_join(tid[ptrs->index][0], NULL);

	char filename[100];
	sprintf(filename, "%dt.out", ptrs->id);
	FILE *fp = fopen(filename, "w+");
	for (int i = 0; i < ptrs->size; i++){
		fprintf(fp, "%s\n", msarr->titles[i]);
		free(msarr->titles[i]);
	}
	free(msarr->titles), free(msarr->scores);
	free(msarr);
}

#elif defined PROCESS

void merge_process(char **ms, double *moviepoints, movienscorensize *ptrs){
	int mid = (ptrs->left + ptrs->right) / 2;
	int lsize = mid - ptrs->left + 1, rsize = ptrs->right - mid;
	char **larr = (char **) malloc(sizeof(char *) * lsize);
	double *larrpoints = (double *) malloc(sizeof(double) * lsize);
	for (int i = 0; i < lsize; i++){
		char *s = (char *) malloc(sizeof(char) * MAX_LEN);
		strcpy(s, ms[ptrs->left+i]);
		larr[i] = s;
		larrpoints[i] = moviepoints[ptrs->left+i];
	}

	char **rarr = (char **) malloc(sizeof(char *) * rsize);
	double *rarrpoints = (double *) malloc(sizeof(double) * rsize);
	for (int i = 0; i < rsize; i++){
		char *s = (char *) malloc(sizeof(char) * MAX_LEN);
		strcpy(s, ms[mid+1+i]);
		rarr[i] = s;
		rarrpoints[i] = moviepoints[mid+1+i];
	}

	int k = ptrs->left;
	int i = 0, j = 0;
	while (i < lsize && j < rsize){
		if (larrpoints[i] > rarrpoints[j] || (larrpoints[i] == rarrpoints[j] && strcmp(larr[i], rarr[j]) < 0)){
			strcpy(ms[k], larr[i]);
			moviepoints[k] = larrpoints[i];
			i++, k++;
		}
		else{
			strcpy(ms[k], rarr[j]);
			moviepoints[k] = rarrpoints[j];
			j++, k++;
		}
	}

	while (i < lsize){
		strcpy(ms[k], larr[i]);
		moviepoints[k] = larrpoints[i];
		i++, k++;
	}
	while (j < rsize){
		strcpy(ms[k], rarr[j]);
		moviepoints[k] = rarrpoints[j];
		j++, k++;
	}
	free(larr), free(rarr);
	free(larrpoints), free(rarrpoints);
}

void merge_sort_process(char **ms, double *moviepoints, movienscorensize *ptrs){
	int left = ptrs->left, right = ptrs->right;
	int mid = (left + right) / 2;
	int depth = ptrs->depth;
	//printf("left = %d, right = %d, depth = %d\n", left, right, depth);
	if (depth < MYDEPTH && left < right){
		pid_t pidl = fork();
		if (pidl == 0){ //child process of left fork
			movienscorensize *ptrsl = (movienscorensize *) malloc(sizeof(movienscorensize));
			memcpy(ptrsl, ptrs, sizeof(movienscorensize));
			ptrsl->left = left, ptrsl->right = mid;
			ptrsl->depth++;
			merge_sort_process(ms, moviepoints, ptrsl);
		}

		pid_t pidr = fork();
		if (pidr == 0){ //child process of right fork
			movienscorensize *ptrsr = (movienscorensize *) malloc(sizeof(movienscorensize));
			memcpy(ptrsr, ptrs, sizeof(movienscorensize));
			ptrsr->left = mid+1, ptrsr->right = right;
			ptrsr->depth++;
			merge_sort_process(ms, moviepoints, ptrsr);
		}

		if (pidl != 0){ //parent process
			wait(NULL);
		}
		if (pidr != 0){ //parent process
			wait(NULL);
		}

		merge_process(ms, moviepoints, ptrs);

	}
	else{ //using sort() library
		int dsize = right - left + 1;
		char **strings = (char **) malloc(sizeof(char *) * dsize);
		double *points = (double *) malloc(sizeof(double) * dsize);
		for (int i = 0; i < dsize; i++){
			strings[i] = (char *) malloc(sizeof(char) * MAX_LEN);
			strcpy(strings[i], ms[i+left]);
			points[i] = moviepoints[i+left];
		}

		sort(strings, points, dsize);
		for (int i = 0; i < dsize; i++){
			strcpy(ms[i+left], strings[i]);
			free(strings[i]);
			moviepoints[i+left] = points[i];
		}

		free(strings);
		free(points);
	}
	exit(0);
}

void process_start(){
	movienscorensize *ptrs = (movienscorensize *) malloc(sizeof(movienscorensize));
	ptrs->size = 0;

	char **ms = (char **) mmap(NULL, sizeof(char *) * num_of_movies, PRFLAG, MFLAG, -1, 0);
	double *moviepoints = (double *) mmap(NULL, sizeof(double) * num_of_movies, PRFLAG, MFLAG, -1, 0);
	
	int fr = pop();
	bool all = false;
	if (strcmp(reqs[fr]->keywords, "*") == 0) all = true;
	
	for (int j = 0; j < num_of_movies; j++){
		if (all){
			char *s = (char *) mmap(NULL, sizeof(char) * MAX_LEN, PRFLAG, MFLAG, -1, 0);
			strcpy(s, movies[j]->title);
			ms[ptrs->size] = s;
			moviepoints[ptrs->size] = calscore(reqs[fr], movies[j]);
			ptrs->size++;
		}
		else{
			if (strstr(movies[j]->title, reqs[fr]->keywords) != NULL){
				char *s = (char *) mmap(NULL, sizeof(char) * MAX_LEN, PRFLAG, MFLAG, -1, 0);
				strcpy(s, movies[j]->title);
				ms[ptrs->size] = s;
				moviepoints[ptrs->size] = calscore(reqs[fr], movies[j]);
				ptrs->size++;
			}
		}
	}
	ptrs->left = 0, ptrs->right = ptrs->size - 1;
	ptrs->id = reqs[fr]->id;
	pid_t forkpid = fork();
	if (forkpid == 0){
		merge_sort_process(ms, moviepoints, ptrs);
	}
	else{
		wait(NULL);
	}	

	char filename[100];
	sprintf(filename, "%dp.out", ptrs->id);
	FILE *fp = fopen(filename, "w+");
	for (int i = 0; i < ptrs->size; i++){
		fprintf(fp, "%s\n", ms[i]);
	}
}
#endif

int main(int argc, char *argv[]){

	if(argc != 1){
#ifdef PROCESS
		fprintf(stderr,"usage: ./pserver\n");
#elif defined THREAD
		fprintf(stderr,"usage: ./tserver\n");
#endif
		exit(-1);
	}
	FILE *fp;

	if ((fp = fopen("./data/movies.txt","r")) == NULL){
		ERR_EXIT("fopen");
	}

	initialize(fp);
	assert(fp != NULL);


	#ifdef PROCESS
	process_start();
	#elif defined THREAD

	pthread_t npid[num_of_reqs]; //handle multiple requests
	for (int i = 0; i < num_of_reqs; i++){
		pthread_create(&npid[i], NULL, thread_start, NULL);
	}
	for (int i = 0; i < num_of_reqs; i++){
		pthread_join(npid[i], NULL);
	}
	#endif


	fclose(fp);	

	return 0;
}

/**=======================================
 * You don't need to modify following code *
 * But feel free if needed.                *
 =========================================**/

request* read_request(){
	int id;
	char buf1[MAX_LEN],buf2[MAX_LEN];
	char delim[2] = ",";

	char *keywords;
	char *token, *ref_pts;
	char *ptr;
	double ret,sum;

	scanf("%u %254s %254s",&id,buf1,buf2);
	keywords = malloc(sizeof(char)*strlen(buf1)+1);
	if(keywords == NULL){
		ERR_EXIT("malloc");
	}

	memcpy(keywords, buf1, strlen(buf1));
	keywords[strlen(buf1)] = '\0';

	double* profile = malloc(sizeof(double)*NUM_OF_GENRE);
	if(profile == NULL){
		ERR_EXIT("malloc");
	}
	sum = 0;
	ref_pts = strtok(buf2,delim);
	for(int i = 0;i < NUM_OF_GENRE;i++){
		ret = strtod(ref_pts, &ptr);
		profile[i] = ret;
		sum += ret*ret;
		ref_pts = strtok(NULL,delim);
	}

	// normalize
	sum = sqrt(sum);
	for(int i = 0;i < NUM_OF_GENRE; i++){
		if(sum == 0)
				profile[i] = 0;
		else
				profile[i] /= sum;
	}

	request* r = malloc(sizeof(request));
	if(r == NULL){
		ERR_EXIT("malloc");
	}

	r->id = id;
	r->keywords = keywords;
	r->profile = profile;

	return r;
}

/*=================initialize the dataset=================*/
void initialize(FILE* fp){

	char chunk[MAX_LEN] = {0};
	char *token,*ptr;
	double ret,sum;
	int cnt = 0;

	assert(fp != NULL);

	// first row
	if(fgets(chunk,sizeof(chunk),fp) == NULL){
		ERR_EXIT("fgets");
	}

	memset(movies,0,sizeof(movie_profile*)*MAX_MOVIES);	

	while(fgets(chunk,sizeof(chunk),fp) != NULL){
		
		assert(cnt < MAX_MOVIES);
		chunk[MAX_LEN-1] = '\0';

		const char delim1[2] = " "; 
		const char delim2[2] = "{";
		const char delim3[2] = ",";
		unsigned int movieId;
		movieId = atoi(strtok(chunk,delim1));

		// title
		token = strtok(NULL,delim2);
		char* title = malloc(sizeof(char)*strlen(token)+1);
		if(title == NULL){
			ERR_EXIT("malloc");
		}
		
		// title.strip()
		memcpy(title, token, strlen(token)-1);
	 	title[strlen(token)-1] = '\0';

		// genres
		double* profile = malloc(sizeof(double)*NUM_OF_GENRE);
		if(profile == NULL){
			ERR_EXIT("malloc");
		}

		sum = 0;
		token = strtok(NULL,delim3);
		for(int i = 0; i < NUM_OF_GENRE; i++){
			ret = strtod(token, &ptr);
			profile[i] = ret;
			sum += ret*ret;
			token = strtok(NULL,delim3);
		}

		// normalize
		sum = sqrt(sum);
		for(int i = 0; i < NUM_OF_GENRE; i++){
			if(sum == 0)
				profile[i] = 0;
			else
				profile[i] /= sum;
		}

		movie_profile* m = malloc(sizeof(movie_profile));
		if(m == NULL){
			ERR_EXIT("malloc");
		}

		m->movieId = movieId;
		m->title = title;
		m->profile = profile;

		movies[cnt++] = m;
	}
	num_of_movies = cnt;

	// request
	scanf("%d",&num_of_reqs);
	assert(num_of_reqs <= MAX_REQ);
	for(int i = 0; i < num_of_reqs; i++){
		reqs[i] = read_request();
	}
}
/*========================================================*/
