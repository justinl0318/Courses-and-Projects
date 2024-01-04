#include <bits/stdc++.h>
#include <stdbool.h>
using namespace std;
#define MAXM 100000

struct node{
    int value;
    bool notflag;
};

bool checkclause(int value1, bool notflag1, int value2, bool notflag2, int value3, bool notflag3){
    if (notflag1){
        value1++;
        value1 %= 2;
    }
    if (notflag2){
        value2++;
        value2 %= 2;
    }
    if (notflag3){
        value3++;
        value3 %= 2;
    }
    if (value1 == 1 || value2 == 1 || value3 == 1) return true;
    return false;
}

node nodearr[MAXM][3];

int main(){
    int n, m;
    cin >> n >> m;
    for (int i = 0; i < m; i++){
        int a, b, c;
        cin >> a >> b >> c;
        nodearr[i][0] = {abs(a), false};
        nodearr[i][1] = {abs(b), false};
        nodearr[i][2] = {abs(c), false};
        if (abs(a) == -a){
            nodearr[i][0].notflag = true;
        }
        if (abs(b) == -b){
            nodearr[i][1].notflag = true;
        }
        if (abs(c) == -c){
            nodearr[i][2].notflag = true;
        }
    }

    int varr[n];
    int totalcount = 0;
    srand(time(0));
    while (1){
        for (int j = 0; j < n; j++){
            varr[j] = rand() % 2;
        }

        for (int i = 0; i < m; i++){
            int val1 = varr[nodearr[i][0].value-1];
            bool f1 = nodearr[i][0].notflag;
            int val2 = varr[nodearr[i][1].value-1];
            bool f2 = nodearr[i][1].notflag;
            int val3 = varr[nodearr[i][2].value-1];
            bool f3 = nodearr[i][2].notflag;
            if (checkclause(val1, f1, val2, f2, val3, f3)){
                totalcount++;
            }
        }

        if (totalcount >= (double) m * 7 * 0.125){
            for (int j = 0; j < n; j++){
                if (varr[j] == 1) printf("1");
                else printf("-1");
                if (j != n-1) printf(" ");
            }
            printf("\n");
            break;
        }

        totalcount = 0;
    }
    return 0;
}