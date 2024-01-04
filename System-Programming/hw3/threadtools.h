#ifndef THREADTOOL
#define THREADTOOL
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/signal.h>


#define THREAD_MAX 16  // maximum number of threads created
#define BUF_SIZE 512
struct tcb {
    int id;  // the thread id
    jmp_buf environment;  // where the scheduler should jump to
    int arg;  // argument to the function
    int fd;  // file descriptor for the thread
    char buf[BUF_SIZE];  // buffer for the thread
    int i, x, y;  // declare the variables you wish to keep between switches
};

extern int timeslice;
extern jmp_buf sched_buf;
extern struct tcb *ready_queue[THREAD_MAX], *waiting_queue[THREAD_MAX];
/*
 * rq_size: size of the ready queue
 * rq_current: current thread in the ready queue
 * wq_size: size of the waiting queue
 */
extern int rq_size, rq_current, wq_size;
/*
* base_mask: blocks both SIGTSTP and SIGALRM
* tstp_mask: blocks only SIGTSTP
* alrm_mask: blocks only SIGALRM
*/
extern sigset_t base_mask, tstp_mask, alrm_mask;

/*
 * Use this to access the running thread.
 */
#define RUNNING (ready_queue[rq_current])

void sighandler(int signo);
void scheduler();

#define thread_create(func, id, arg) {\
    func(id, arg);\
}\

#define thread_setup(id, arg) {\
    struct tcb *mytcb = (struct tcb *) malloc(sizeof(struct tcb));\
    mytcb->id = id;\
    mytcb->arg = arg;\
    char fifoname[BUF_SIZE];\
    sprintf(fifoname, "%d_%s", id, __func__);\
    mkfifo(fifoname, 0777);\
    mytcb->fd = open(fifoname, O_RDONLY | O_NONBLOCK);\
    ready_queue[rq_size] = mytcb;\
    rq_size++;\
    if (setjmp(mytcb->environment) == 0){\
        return;\
    }\
}

#define thread_exit() {\
    char fifoname[BUF_SIZE];\
    sprintf(fifoname, "%d_%s", RUNNING->id, __func__);\
    unlink(fifoname);\
    longjmp(sched_buf, 3);\
}

#define thread_yield() {\
    if (setjmp(RUNNING->environment) == 0){\
        sigprocmask(SIG_UNBLOCK, &tstp_mask, NULL);\
        sigprocmask(SIG_BLOCK, &tstp_mask, NULL);\  
        sigprocmask(SIG_UNBLOCK, &alrm_mask, NULL);\
        sigprocmask(SIG_BLOCK, &alrm_mask, NULL);\
    }\
}

#define async_read(count) ({\
    if (setjmp(RUNNING->environment) == 0){\
        longjmp(sched_buf, 2);\
    }\
    read(RUNNING->fd, RUNNING->buf+1, count-1);\
})

#endif // THREADTOOL
