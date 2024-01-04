#include <bits/stdc++.h>
using namespace std;

int dp[3001][3001];

int min2(int x, int y){
    if (x < y) return x;
    return y;
}

int min3(int x, int y, int z){
    return min2(min2(x, y), min2(y, z));
}

int computedp(string s1, string s2, int m, int n){
    for (int i = 0; i <= m; i++){
        for (int j = 0; j <= n; j++){
            if (i == 0){
                dp[i][j] = j;
            }
            else if (j == 0){
                dp[i][j] = i;
            }
            else if (s1[i-1] == s2[j-1]){
                dp[i][j] = dp[i-1][j-1];
            }
            else{
                dp[i][j] = 1 + min3(dp[i-1][j], dp[i][j-1], dp[i-1][j-1]);
            }
        }
    }
    return dp[m][n];
}

int main(){
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    string s1, s2;
    cin >> s1 >> s2;
    cout << computedp(s1, s2, s1.length(), s2.length());
    return 0;
}