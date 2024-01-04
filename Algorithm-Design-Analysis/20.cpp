#include <bits/stdc++.h>
using namespace std;
#define lli long long int
#define LEN 1000000

typedef struct node{
    lli x;
    lli index;
}Node;

bool cmp(Node a, Node b){
    return a.x > b.x;
}

bool cmp2(Node a, node b){
    return a.index < b.index;
}

lli arr[LEN];
Node temp[LEN];
lli len = 0;

lli myceil(lli num, lli k){
    if (num % k == 0) return num / k;
    return (num / k) + 1;
}

void put(lli num){
    if (len == 0){
        arr[len] = num;
    }
    else{
        arr[len] = arr[len-1] + num;
    }
    len++;
}

void take(){
    if (len == 1){
        printf("%lld\n", arr[0]);
    }
    else{
        printf("%lld\n", arr[len-1] - arr[len-2]);
    }
    len--;
}

void cc(lli num, lli k){
    lli lentemp = 0;
    for (lli i = len-num; i < len; i++){
        Node node;
        if (arr[i] == 0){
            node.x = arr[i];
        }
        else{
            node.x = arr[i] - arr[i-1];
        }
        node.index = i;
        temp[lentemp++] = node;
    }
    
    len -= num;
    sort(temp, temp+lentemp, cmp);
    lli index = myceil(num, k);

    lli sum = 0;
    for (lli i = 0; i < index; i++){
        sum += temp[i].x; 
    }
    sort(temp+index, temp+lentemp, cmp2);
    for (lli i = index; i < lentemp; i++){
        put(temp[i].x);
    }
    printf("%lld\n", sum);
}  

void dd(lli num){
    if (len == num){
        printf("%lld\n", arr[len-1]);
    }
    else{
        printf("%lld\n", arr[len-1] - arr[len-1-num]);
    }
}

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    lli m;
    cin >> m;
    string s;
    lli num;
    lli k, value;
    for (lli i = 0; i < m; i++){
        cin >> s;
        if (s[0] == 'P'){
            cin >> num;
            put(num);
        }
        else if (s[0] == 'T'){
            take();
        }
        else if (s[0] == 'C'){
            cin >> num >> k;
            cc(num, k);
        }
        else if (s[0] == 'D'){
            cin >> num;
            dd(num);
        }
    }
    return 0;
}