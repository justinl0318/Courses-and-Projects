#include "kernel/types.h"
#include "user/setjmp.h"
#include "user/threads.h"
#include "user/user.h"
#define NULL 0


static struct thread* current_thread = NULL;
static int id = 1;
static jmp_buf env_st;
static jmp_buf env_tmp;

struct thread *thread_create(void (*f)(void *), void *arg){
    struct thread *t = (struct thread*) malloc(sizeof(struct thread));
    unsigned long new_stack_p;
    unsigned long new_stack;
    new_stack = (unsigned long) malloc(sizeof(unsigned long)*0x100);
    new_stack_p = new_stack +0x100*8-0x2*8;
    t->fp = f;
    t->arg = arg;
    t->ID  = id;
    t->buf_set = 0;
    t->stack = (void*) new_stack;
    t->stack_p = (void*) new_stack_p;
    id++;

    // part 2
    t->sig_handler[0] = NULL_FUNC;
    t->sig_handler[1] = NULL_FUNC;
    t->signo = -1;
    t->handler_buf_set = 0;
    return t;
}

void thread_add_runqueue(struct thread *t){
    t->previous = NULL;
    t->next = NULL;
    if(current_thread == NULL){
        t->previous = t->next = t;
        current_thread = t;
    }
    else{
        t->previous = current_thread->previous;
        current_thread->previous->next = t;
        t->next = current_thread;
        current_thread->previous = t;
    }

    //child inherit signal handlers from current_thread
    t->sig_handler[0] = current_thread->sig_handler[0];
    t->sig_handler[1] = current_thread->sig_handler[1];
}

void thread_yield(void){
    if (current_thread->signo == -1){ //thread_yield is called from thread function
        if (setjmp(current_thread->env) == 0){
            current_thread->buf_set = 1;
            schedule();
            dispatch();
        }
    }
    else{ //thread yield called from sighandler
        if (setjmp(current_thread->handler_env) == 0){
            current_thread->handler_buf_set = 1;
            schedule();
            dispatch();
        }
    }
}

void dispatch(void){

    if (current_thread->signo != -1){ //a signal comes
        if (current_thread->sig_handler[current_thread->signo] == NULL_FUNC){
            thread_exit();
        }
        else{
            if (current_thread->handler_buf_set == 0){ //sighandler needs initialization
                if (setjmp(env_tmp) == 0){
                    //allocate a new stack for the handler function
                    //如果沒有allocate new stack，假如是thread function 先跑，中途變成handler function
                    //這時handler function 就會把原本thread function 的stack frame 覆蓋掉，這樣 handler
                    //跑完要回來 thread function時，thread function 的stack frame 就會不見。所以要各自
                    //allocate new stack，這樣stack frame 才不會互相覆蓋掉。
                    unsigned long handler_stack = (unsigned long) malloc(sizeof(unsigned long)*0x100);
                    unsigned long handler_stack_p = handler_stack + 0x100*8-0x2*8;
                    env_tmp->sp = (unsigned long) handler_stack_p;
                    longjmp(env_tmp, 1);
                }
                else{
                    current_thread->sig_handler[current_thread->signo](current_thread->signo); //call handler function
                    current_thread->signo = -1; //reset signo, handler function returns
                }
            }
            else{
                longjmp(current_thread->handler_env, 1);
            }
        }
    }
    
    if (current_thread->signo == -1){ //execute thread function
        if (current_thread->buf_set == 0){ //thread has never run b4, need initialization
            if (setjmp(env_tmp) == 0){
                env_tmp->sp = (unsigned long) current_thread->stack_p;
                longjmp(env_tmp, 1);
            }
            else{
                current_thread->fp(current_thread->arg); //call function
                thread_exit();
            }
        }
        else{
            longjmp(current_thread->env, 1);
        }
    }
}

void schedule(void){
    current_thread = current_thread->next;
}

void thread_exit(void){
    if(current_thread->next != current_thread){
        struct thread *exit_thread = current_thread;
        current_thread = current_thread->next;
        
        exit_thread->previous->next = exit_thread->next;
        exit_thread->next->previous = exit_thread->previous;
        free(exit_thread->stack);
        free(exit_thread);
        dispatch();
    }
    else{ //last thread in queue
        free(current_thread->stack), free(current_thread);
        current_thread = NULL;
        longjmp(env_st, 1);
    }
}

void thread_start_threading(void){
    if (setjmp(env_st) == 0){ //return to main if longjmp from thread_exit
        dispatch();
    }
}

// part 2
void thread_register_handler(int signo, void (*handler)(int)){
    current_thread->sig_handler[signo] = handler; //register thread's signal handler for signo
}

void thread_kill(struct thread *t, int signo){
    t->signo = signo; //send signal to t
}