#include "threadtools.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

int max3(int a, int b, int c){
    if (a >= b && a >= c) return a;
    if (b >= a && b >= c) return b;
    return c;
}

void fibonacci(int id, int arg) {
    thread_setup(id, arg);
    for (RUNNING->i = 1; ; RUNNING->i++) {
        if (RUNNING->i <= 2)
            RUNNING->x = RUNNING->y = 1;
        else {
            /* We don't need to save tmp, so it's safe to declare it here. */
            int tmp = RUNNING->y;  
            RUNNING->y = RUNNING->x + RUNNING->y;
            RUNNING->x = tmp;
        }
        printf("%d %d\n", RUNNING->id, RUNNING->y);
        fflush(stdout);
        sleep(1);

        if (RUNNING->i == RUNNING->arg) {
            thread_exit();
        }
        else {
            thread_yield();
        }
    }
}
  
void collatz(int id, int arg) {
    thread_setup(id, arg);
    RUNNING->i = RUNNING->arg;
    while (true){
        if (RUNNING->i % 2 == 0){
            RUNNING->i /= 2;
        }
        else{
            RUNNING->i = RUNNING->i * 3 + 1;
        }
        printf("%d %d\n", RUNNING->id, RUNNING->i);
        fflush(stdout);
        sleep(1);
        if (RUNNING->i == 1){
            thread_exit();
        }
        else{
            thread_yield();
        }
    }
}

void max_subarray(int id, int arg) {
    
    thread_setup(id, arg);
    RUNNING->x = 0, RUNNING->y = INT32_MIN;
    for (RUNNING->i = 0; ;){
        async_read(5);
        RUNNING->i++;
        RUNNING->x = max3(atoi(RUNNING->buf), atoi(RUNNING->buf)+RUNNING->x, 0);
        if (RUNNING->x > RUNNING->y){
            RUNNING->y = RUNNING->x;
        }
        printf("%d %d\n", RUNNING->id, RUNNING->y);
        memset(RUNNING->buf, '\0', sizeof(RUNNING->buf));
        sleep(1);
        if (RUNNING->i == RUNNING->arg){
            thread_exit();
        }
        else{
            thread_yield();
        }
    }

}
