#include <bits/stdc++.h>
using namespace std;
#define LEN 1005
#define lli long long int
lli arr[LEN];
lli dp[LEN][LEN];
lli table[LEN][LEN];

lli cmaxsub(lli start, lli end){
    lli ans = arr[start], curr = arr[start];
    for (lli i = start+1; i <= end; i++){
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
    return ans;
}

lli minimumsum(lli n, lli index, lli k, lli dp[LEN][LEN]){
    if (k == 0){
        if (index == n){
            return 0;
        }
        else return 214748364700000;
    }

    else if (index == n){
        return 214748364700000;
    }

    
    else if (dp[index][k] != -1){
        return dp[index][k];
    }

    else{
        lli ans = 214748364700000;
        for (lli i = index; i < n; i++){
            if (k == 1){
                i = n-1;
            }
            if ((k == 1 && i+1 == n) || k > 1){
                ans = min(ans, minimumsum(n, i+1, k-1, dp) + table[index][i] * (i - index + 1));
            }
        }
        return dp[index][k] = ans;
    }
}

void fastscan(lli &number)
{
    bool negative = false;
    register lli c;
    number = 0;
    c = getchar();
    if (c=='-'){
        negative = true;
        c = getchar();
    }

    for (; (c>47 && c<58); c=getchar())
        number = number *10 + c - 48;

    if (negative)
        number *= -1;
}

int main(){
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    lli n, k;
    fastscan(n);
    fastscan(k);

    for (lli i = 0; i < n; i++){
        lli num;
        fastscan(num);
        arr[i] = num;
    }

    for (int i = 0; i < n; i++){
        for (int j = i; j < n; j++){
            table[i][j] = cmaxsub(i, j);
        }
    }

    memset(dp, -1, sizeof(dp));
    cout << minimumsum(n, 0, k, dp);
    return 0;
}