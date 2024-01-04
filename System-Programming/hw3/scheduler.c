#include "threadtools.h"
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/signal.h>

int arr[THREAD_MAX];

void waiting2ready(){
    memset(arr, 0, sizeof(arr));
    int readret;
    for (int i = 0; i < wq_size; i++){
        readret = read(waiting_queue[i]->fd, waiting_queue[i]->buf, 1);
        if (readret == 1){
            ready_queue[rq_size] = waiting_queue[i];
            arr[i] = 1;
            rq_size++;
        }
    }
    int index = 0;
    for (int i = 0; i < wq_size; i++){
        if (arr[i] == 0){ //not hole
            waiting_queue[index] = waiting_queue[i];
            index++;
        }
        arr[i] = 0;
    }
    wq_size = index;
}

/*
 * Print out the signal you received.
 * If SIGALRM is received, reset the alarm here.
 * This function should not return. Instead, call longjmp(sched_buf, 1).
 */
void sighandler(int signo) {
    if (signo == SIGTSTP){
        printf("caught SIGTSTP\n");
        fflush(stdout);
    }
    else if (signo == SIGALRM){
        printf("caught SIGALRM\n");
        fflush(stdout);
        alarm(timeslice);
    }
    longjmp(sched_buf, 1);
    return;
}

/*
 * Prior to calling this function, both SIGTSTP and SIGALRM should be blocked.
 */
void scheduler() {
    int ret;
    ret = setjmp(sched_buf);
    if (ret == 1){ //from sighandler, caught signal (thread yield)
        waiting2ready();
        if (rq_size == 0){
            while (rq_size == 0){
                waiting2ready();
            }
        }
        if (rq_current == rq_size - 1) rq_current = 0;
        else rq_current++;
    }
    else if (ret == 2){ //async read
        waiting2ready();
        waiting_queue[wq_size++] = RUNNING;
        if (rq_size == 1){
            rq_size--;
            while (rq_size == 0){
                waiting2ready();
            }
        }
        else{
            if (rq_current == rq_size - 1){
                rq_size--, rq_current = 0;
            }
            else{
                ready_queue[rq_current] = ready_queue[rq_size-1];
                rq_size--;
            }
        }
    }
    else if (ret == 3){ //thread exit
        waiting2ready();
        if (rq_size == 1){ //only one tcb left in ready queue
            rq_size--;
            if (wq_size == 0){//waiting queue also empty
                exit(0);
            }
            while (rq_size == 0){
                waiting2ready();
            }
        }
        else{
            if (rq_current == rq_size - 1){ //exit tcb is the last thread in queue, so execute the first thread
                free(ready_queue[rq_current]);
                rq_size--;
                rq_current = 0;
            }
            else{ //not last thread in queue
                free(ready_queue[rq_current]);
                ready_queue[rq_current] = ready_queue[rq_size-1];
                rq_size--;
            }
        }
    }
    longjmp(RUNNING->environment, 1);
}
