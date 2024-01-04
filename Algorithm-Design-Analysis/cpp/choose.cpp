#include "choose.h"
#include <bits/stdc++.h>
using namespace std;

/*
Feel free to use global variables.
However, if you want to write your code more "correctly",
you can search for the keyword "anonymous namespace", a helpful tool in C++.
*/

// this is an example usage

bool cmp(int a, int b){
    if (compare_task(a, b) == 1) return true;
    return false;
}

void schedule(int N){
    int arr[N+1];
    for (int i = 1; i <= N; i++){
        arr[i] = i;
    }
    sort(arr+1, arr+N+1, cmp);
    int endtime[3] = {0, 0, 0};
    for (int i = 1; i <= N; i++){
        if (endtime[1] <= endtime[2]){
            endtime[1] = assign_task(1, arr[i]);
        }
        else{
            endtime[2] = assign_task(2, arr[i]);
        }
    }
}
