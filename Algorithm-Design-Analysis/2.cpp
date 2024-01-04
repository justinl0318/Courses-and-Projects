#include <iostream>
#include <bits/stdc++.h>
#include <stdio.h>
using namespace std;
#define lli long long int

int main(){
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    lli n;
    cin >> n;
    lli arr[n];
    lli num;
    for (lli i = 0; i < n; i++){
        cin >> num;
        arr[i] = num;
    }

    lli ans = arr[0], curr = arr[0];
    for (lli i = 1; i < n; i++){
        curr += arr[i];
        if (curr < arr[i]){
            curr = arr[i];
        }
        if (curr < 0){
            curr = arr[i];
        }
        if (ans < curr){
            ans = curr;
        }
    }
    cout << ans << "\n";

    return 0;
}