#include <bits/stdc++.h>

using namespace std;

vector<int> v;

void recur(int l, int r){
    if (l==r){
        return;
    }
    int mid = (l+r)/2;
    recur(l, mid);
    for (int i=l; i <=r; i++){
        v.push_back(-1);
    }
    for (int i=mid; i >=l; i--){
        v.push_back(i);
    }
    for (int i=r; i>=mid+1; i--){
        v.push_back(i);
    }
    v.push_back(-2);
    recur(mid+1, r);
}

int main()
{
    int n;
    cin >> n;
    int lines = 0;
    for (int i= n; i>=1; i--){
        v.push_back(i);
        lines++;
    }
    v.push_back(-2);
    lines++;
    recur(1, n);
    printf("%ld\n", v.size());
    for (int i = 0; i < v.size(); i++){
        if (v[i] == -2){
            printf("PHOTO\n");
        }
        else if (v[i] == -1){
            printf("POP\n");
        }
        else{
            printf("PLACE %d\n", v[i]);
        }
    }
    return 0;
}
