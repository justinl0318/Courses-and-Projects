#include "kernel/types.h"
#include "user/user.h"
#include "user/list.h"
#include "user/threads.h"
#include "user/threads_sched.h"

#define NULL 0
#define FALSE 0
#define TRUE 1

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MYINT32_MAX 2147483647

/* default scheduling algorithm */
struct threads_sched_result schedule_default(struct threads_sched_args args)
{
    struct thread *thread_with_smallest_id = NULL;
    struct thread *th = NULL;
    list_for_each_entry(th, args.run_queue, thread_list) {
        if (thread_with_smallest_id == NULL || th->ID < thread_with_smallest_id->ID) {
            thread_with_smallest_id = th;
        }
    }

    struct threads_sched_result r;
    if (thread_with_smallest_id != NULL) {
        r.scheduled_thread_list_member = &thread_with_smallest_id->thread_list;
        r.allocated_time = thread_with_smallest_id->remaining_time;
    } else {
        r.scheduled_thread_list_member = args.run_queue;
        r.allocated_time = 1;
    }

    return r;
}

int edf_find_allocated_time(struct threads_sched_args args, struct thread *edthread){
    int allocated_time;
    //determine if we found a thread in releaseq whose next deadline is <= than edthread's current deadline
    int found_flag = FALSE; 

    //find min next deadline in releaseq
    int min_nextdeadline_in_releaseq = edthread->current_deadline;
    int min_id = edthread->ID;
    int min_deadline_in_releaseq;
    struct release_queue_entry *temp = NULL;
    list_for_each_entry(temp, args.release_queue, thread_list){
        if (temp->thrd->current_deadline + temp->thrd->period < min_nextdeadline_in_releaseq){
            min_nextdeadline_in_releaseq = temp->thrd->current_deadline + temp->thrd->period;
            min_id = temp->thrd->ID;
            min_deadline_in_releaseq = temp->thrd->current_deadline;
            found_flag = TRUE;
        }
        else if (temp->thrd->current_deadline + temp->thrd->period == min_nextdeadline_in_releaseq && temp->thrd->ID < min_id){
            min_id = temp->thrd->ID;
            min_deadline_in_releaseq = temp->thrd->current_deadline;
            found_flag = TRUE;
        }
    }

    //flag = TRUE: current thread should get preempted when the higher priority thread comes
    if (found_flag == TRUE){
        allocated_time = MIN(min_deadline_in_releaseq - args.current_time, edthread->remaining_time);
    }
    else{
        allocated_time = edthread->remaining_time;
    }

    //if the thread I want to dispatch will miss its deadline when it is running
    if (args.current_time + allocated_time > edthread->current_deadline){
        allocated_time = edthread->current_deadline - args.current_time;
    }
    return allocated_time;
}


int rm_find_allocated_time(struct threads_sched_args args, struct thread *rmthread){
    int allocated_time;
    //determine if we found a flag in the releaseq whose deadline is <= rmthread's current deadline
    //and also its period is <= rmthread's period
    int found_flag = FALSE; 

    int min_deadline_in_releaseq = rmthread->current_deadline;
    int min_period = rmthread->period;
    int min_id = rmthread->ID;
    struct release_queue_entry *temp = NULL;
    list_for_each_entry(temp, args.release_queue, thread_list){
        if (temp->thrd->current_deadline < min_deadline_in_releaseq){
            if (temp->thrd->period < min_period || (temp->thrd->period == min_period && temp->thrd->ID < min_id)){
                min_deadline_in_releaseq = temp->thrd->current_deadline;
                min_id = temp->thrd->ID;
                min_period = temp->thrd->period;
                found_flag = TRUE;
            }
        }
        //try to delete this part later
        else if (temp->thrd->current_deadline == min_deadline_in_releaseq){
            if (temp->thrd->period < min_period || (temp->thrd->period == min_period && temp->thrd->ID < min_id)){
                min_id = temp->thrd->ID;
                min_period = temp->thrd->period;
                found_flag = TRUE;
            }
        }
    }

    //flag = TRUE: current thread should get preempted when the higher priority thread comes
    if (found_flag == TRUE){
        allocated_time = MIN(min_deadline_in_releaseq - args.current_time, rmthread->remaining_time);
    }
    else{
        allocated_time = rmthread->remaining_time;
    }
    
    //if the thread I want to dispatch will miss its deadline when it is running
    if (args.current_time + allocated_time > rmthread->current_deadline){
        allocated_time = rmthread->current_deadline - args.current_time;
    }
    return allocated_time;
}


void check_missed_deadline_in_runq(struct threads_sched_args args, struct threads_sched_result *r, struct thread *edthread){
    struct thread *missed_deadline_thread = NULL;
    struct thread *temp = NULL;
    list_for_each_entry(temp, args.run_queue, thread_list){
        if (temp->current_deadline <= args.current_time){
            if (missed_deadline_thread == NULL){
                missed_deadline_thread = temp;
            }
            else if (missed_deadline_thread != NULL && temp->ID < missed_deadline_thread->ID){
                missed_deadline_thread = temp;
            }
        }
    }

    //if we find thread that missed its deadline
    if (missed_deadline_thread != NULL){
        r->scheduled_thread_list_member = &missed_deadline_thread->thread_list;
        r->allocated_time = 0;
    }
    return;
}

int allocate_time_when_runq_empty(struct threads_sched_args args){
    int earliest_deadline_in_releaseq = MYINT32_MAX;
    struct release_queue_entry *temp = NULL;
    list_for_each_entry(temp, args.release_queue, thread_list){
        if (temp->thrd->current_deadline < earliest_deadline_in_releaseq){
            earliest_deadline_in_releaseq = temp->thrd->current_deadline;
        }
    }
    return earliest_deadline_in_releaseq - args.current_time;
}

/* Earliest-Deadline-First scheduling */
struct threads_sched_result schedule_edf(struct threads_sched_args args)
{
    struct thread *earliest_deadline_thread = NULL;
    struct thread *th = NULL;
    list_for_each_entry(th, args.run_queue, thread_list){
        if (earliest_deadline_thread == NULL || th->current_deadline < earliest_deadline_thread->current_deadline
            || (th->current_deadline == earliest_deadline_thread->current_deadline && th->ID < earliest_deadline_thread->ID)){
            earliest_deadline_thread = th;
        }
    }

    struct threads_sched_result r;
    if (earliest_deadline_thread != NULL){
        r.scheduled_thread_list_member = &earliest_deadline_thread->thread_list;
        r.allocated_time = edf_find_allocated_time(args, earliest_deadline_thread);
        check_missed_deadline_in_runq(args, &r, earliest_deadline_thread);
    }
    else{ //run_queue is empty
        r.scheduled_thread_list_member = args.run_queue;
        r.allocated_time = allocate_time_when_runq_empty(args);
    }

    return r;
}

/* Rate-Monotonic Scheduling */
struct threads_sched_result schedule_rm(struct threads_sched_args args)
{
    struct thread *min_period_thread = NULL;
    struct thread *th = NULL;
    list_for_each_entry(th, args.run_queue, thread_list){
        if (min_period_thread == NULL || th->period < min_period_thread->period
            || (th->period == min_period_thread->period && th->ID < min_period_thread->ID)){
                min_period_thread = th;
        }
    }

    struct threads_sched_result r;
    if (min_period_thread != NULL){
        r.scheduled_thread_list_member = &min_period_thread->thread_list;
        r.allocated_time = rm_find_allocated_time(args, min_period_thread);
        check_missed_deadline_in_runq(args, &r, min_period_thread);
    }
    else{ //runq is empty
        r.scheduled_thread_list_member = args.run_queue;
        r.allocated_time = allocate_time_when_runq_empty(args);
    }
    return r;
}