#include <bits/stdc++.h>
using namespace std;
#define lli long long int

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    lli n, k, m;
    cin >> n >> k >> m;
    lli dp[2][k+1][k+1];
    for (int j = 0; j <= k; j++){
        for (int i = 0; i <= k; i++){
            if (i < j) dp[0][j][i] = 0;
            else dp[0][j][i] = 1;
        }
    }

    lli columnsum[k+1][k+1];
    for (int j = 0; j <= k; j++){
        for (int i = j; i <= k; i++){
            if (j == 0){
                columnsum[j][i] = 0;
                continue;
            }
            columnsum[j][i] = columnsum[j-1][i] + dp[0][j-1][i];
        }
    }

    lli rowsum[k+1][k+1];
    for (int j = 0; j <= k; j++){
        for (int i = j; i <= k; i++){
            if (i == 0){
                rowsum[j][i] = 0;
                continue;
            }
            rowsum[j][i] = rowsum[j][i-1] + dp[0][j][i-1];
        }
    }

    int index;
    for (int r = 1; r <= n; r++){

        index = r % 2;
        int prev;
        if (index == 1) prev = 0;
        else prev = 1;

        for (int j = 0; j <= k; j++){
            for (int i = j; i <= k; i++){
                if (i < j) continue;
                if (j == 0){
                    columnsum[j][i] = 0;
                }
                else{
                    columnsum[j][i] = (columnsum[j-1][i] + dp[prev][j-1][i])%m;
                }

                if (i == 0){
                    rowsum[j][i] = 0;
                }
                else{
                    if (i == j){
                        rowsum[j][i] = 0;
                    }
                    else
                        rowsum[j][i] = (rowsum[j][i-1] + dp[prev][j][i-1])%m;
                }

                dp[index][j][i] = (dp[prev][j][i] + columnsum[j][i] + rowsum[j][i]) % m;
        
            }
        }
    }
    printf("%lld\n", dp[index][k][k]);
    return 0;
}