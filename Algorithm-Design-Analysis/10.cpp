#include <bits/stdc++.h>
using namespace std;
#define lli long long int
#define SIZE 1000005

struct pos{
    lli x;
    lli y;
};

lli l1, l2, r1, r2;
pos temp[SIZE];
pos arr[SIZE];

bool rcmp(pos a, pos b){
    if ((-r1*a.x + r2*a.y) > (-r1*b.x + r2*b.y)){
        return true;
    }
    else if ((-r1*a.x + r2*a.y) == (-r1*b.x + r2*b.y)){
        if (l1/l2 == r1/r2){
            return true;
        }
        if (l1/l2 <= 0 && r1/r2 >= 0){
            if (a.y < b.y){
                return true;
            }
            else if (a.y == b.y){
                return a.x < b.x;
            }
        }
        
        
        else{
            if (a.y > b.y) return true;
            else if (a.y == b.y) return a.x > b.x;
        }
    }
    return false;
}

bool lcmp(pos a, pos b){
    if ((-l1*a.x + l2*a.y) < (-l1*b.x + l2*b.y)){
        return true;
    }
    else if ((-l1*a.x + l2*a.y) == (-l1*b.x + l2*b.y)){
        if (l1/l2 == r1/r2){
            return true;
        }
        if (l1/l2 <= 0 && r1/r2 >= 0){
            if (a.y > b.y) return true;
            else if (a.y == b.y) return a.x > b.x;
        }
        else{
            if (a.y < b.y) return true;
            else if (a.y == b.y) return a.x < b.x;
        }
        //return a.y > b.y;

    }
    return false;
}

lli mymerge(lli left, lli mid, lli right){
    lli indexl = left, indexr = mid+1, indext = left;
    lli pairs = 0;
    while (indexl <= mid && indexr <= right){
        if (rcmp(arr[indexl], arr[indexr])){
            pairs += (right - indexr) + 1;
            temp[indext] = arr[indexl];
            indext++, indexl++;
        }
        else{
            temp[indext] = arr[indexr];
            indext++, indexr++;
        }
    }

    while (indexl <= mid){
        temp[indext] = arr[indexl];
        indext++, indexl++;
    }
    while (indexr <= right){
        temp[indext] = arr[indexr];
        indext++, indexr++;
    }
    for (int i = left; i <= right; i++){
        arr[i] = temp[i];
    }
    return pairs;
}

lli mymerge2(lli left, lli mid, lli right){
    lli indexl = left, indexr = mid+1, indext = left;
    lli pairs = 0;
    while (indexl <= mid && indexr <= right){
        if (lcmp(arr[indexl], arr[indexr])){
            pairs += (right - indexr) + 1;
            temp[indext] = arr[indexl];
            indext++, indexl++;
        }
        else{
            temp[indext] = arr[indexr];
            indext++, indexr++;
        }
    }

    while (indexl <= mid){
        temp[indext] = arr[indexl];
        indext++, indexl++;
    }
    while (indexr <= right){
        temp[indext] = arr[indexr];
        indext++, indexr++;
    }
    for (int i = left; i <= right; i++){
        arr[i] = temp[i];
    }
    //printf("pairs=%lld\n", pairs);
    return pairs;
}

lli mergesort(lli left, lli right){
    if (left >= right){
        return 0;
    }
    lli pairs = 0;
    lli mid = (left + right) / 2;
    pairs += mergesort(left, mid);
    pairs += mergesort(mid+1, right);
    if (l1/l2 >= 0 && r1/r2 >= 0) pairs += mymerge(left, mid, right);
    else pairs += mymerge2(left, mid, right);
    return pairs;
}


int main(){
    lli n;
    cin >> n;

    cin >> l1 >> l2 >> r1 >> r2;

    for (lli i = 0; i < n; i++){
        lli x, y;
        cin >> x >> y;
        pos newpos = {x, y};
        arr[i] = newpos;
    }
    
    //printf("\n");
    if (l1/l2 >= 0 && r1/r2 >= 0) sort(arr, arr+n, lcmp);
    else sort(arr, arr+n, rcmp);
    for (int i = 0; i < n; i++){
        //printf("%lld %lld\n", arr[i].x, arr[i].y);
    }
    //printf("\n");
    lli pairs = mergesort(0, n-1);
    for (int i = 0; i < n; i++){
        //printf("%lld %lld\n", arr[i].x, arr[i].y);
    }

    
    printf("%lld\n", pairs);
    return 0;
}