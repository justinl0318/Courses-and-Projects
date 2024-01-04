#include <bits/stdc++.h>
#include <stdbool.h>
using namespace std;
#define MAXN 5001
#define MAXW 5001

int results[MAXN][MAXW];

int max(int a, int b){
    return a > b ? a : b;
}

bool cmp(int a, int b){
    return a > b;
}

void init(int n, int w){
    for (int i = 0; i <= n; i++){
        for (int j = 0; j <= w; j++){
            results[i][j] = 0;
        }
    }
}

void myprint(int n, int w){
    for (int i = 0; i <= n; i++){
        for (int j = 0; j <= w; j++){
            printf("%d ", results[i][j]);
        }
        printf("\n");
    }
}

void ideal(int n, int w, int prices[]){
    for (int row = 1; row <= n; row++){
        for (int col = 1; col <= w; col++){
            if (prices[row-1] > col){
                results[row][col] = results[row-1][col];
            }
            else{
                int sol1 = results[row-1][col];
                int sol2 = prices[row-1] + results[row][col-prices[row-1]];
                results[row][col] = max(sol1, sol2);
            }
        }
    }
}

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, w;
    cin >> n >> w;

    int prices[n];
    int price;
    for (int i = 0; i < n; i++){
        cin >> price;
        prices[i] = price;
    }

    sort(prices, prices + n, cmp);
    init(n, w);
    ideal(n, w, prices);
    //myprint(n, w);

    for (int i = 1; i <= w; i++){
        int currmoney = i;
        int brute = 0;
        int index = 0;
        while (index < n){
            if (brute > currmoney){
                break;
            }
            if (brute + prices[index] <= currmoney){
                brute += prices[index];
            }
            else{
                index++;
            }
        }
        cout << results[n][i] - brute << "\n";
        
    }
    return 0;
}