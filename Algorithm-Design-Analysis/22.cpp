#include <bits/stdc++.h>
using namespace std;
#define lli long long int
#define MAXN 3001

lli max2(lli a, lli b){
    if (a >= b) return a;
    return b;
}

lli max3(lli a, lli b, lli c){
    if (a >= b && a >= c) return a;
    else if (b >= a && b >= c) return b;
    return c; 
}

struct Vertex{
    lli num;
    lli ftime;
    lli costc;
    lli costd;
    lli totalcostc;
    lli childscount;
};

bool cmp(Vertex a, Vertex b){
    return a.ftime < b.ftime; //sort vertex in ascending order according to finish time
}

vector<lli> adjlist[MAXN];
bool visited[MAXN], costvisited[MAXN];
Vertex vertexes[MAXN];
lli indexes[MAXN];

lli mytime = 0;
void dfs(lli u){
    mytime = mytime + 1;
    visited[u] = true;
    for (lli i = 0; i < adjlist[u].size(); i++){
        if (visited[adjlist[u][i]] == false){
            dfs(adjlist[u][i]);
        }
    }
    mytime = mytime + 1;
    vertexes[u].ftime = mytime;
}

lli costc = 0;
lli countchild = 0;
void costsum(lli u){
    costvisited[u] = true;
    for (auto i = adjlist[u].begin(); i != adjlist[u].end(); i++){
        if (costvisited[*i] == false){
            lli index = indexes[*i];
            costc += vertexes[index].costc;
            countchild++;
            costsum(*i);
        }
    }
}

int letsgoooo(lli n, lli m){
    lli dp[n+1][m+1]; 
    for (lli i = 0; i < n+1; i++){
        dp[i][0] = 0;
    }

    for (lli i = 0; i < m+1; i++){
        dp[0][i] = 0;
    }

    //i: the number of vertex we have access to, j: amount of dollars we have right now
    for (lli i = 1; i < n+1; i++){
        for (lli j = 1; j < m+1; j++){
            lli choice1 = 0; //no operation
            lli newi = i-1;
            lli index;
            choice1 = max2(choice1, dp[i-1][j]);

            lli maxchoice2 = 0, cost2 = 0; //operation 1
            if (j >= vertexes[newi].costc){
                cost2 = vertexes[newi].costc;
                for (auto k = adjlist[vertexes[newi].num].begin(); k != adjlist[vertexes[newi].num].end(); k++){
                    index = indexes[*k] + 1;
                    maxchoice2 = max2(maxchoice2, vertexes[newi].costc + dp[index][j-cost2]);
                }
                maxchoice2 = max2(maxchoice2, vertexes[newi].costc + dp[i-1][j-cost2]);
            }

            lli maxchoice3 = 0, cost3 = 0; //operation 2
            lli childcount = 0;
            if (j >= vertexes[newi].costd){
                cost3 = vertexes[newi].costd;
                childcount = vertexes[newi].childscount + 1;
                maxchoice3 = vertexes[newi].totalcostc + dp[i-childcount][j-cost3];
            }

            dp[i][j] = max3(choice1, maxchoice2, maxchoice3);
        }
    }
    return dp[n][m];
}

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    lli n, m;
    cin >> n >> m;
    for (lli i = 0; i < n; i++){
        lli parent;
        cin >> parent;
        adjlist[parent].push_back(i);
    }

    for (lli i = 0; i < n; i++){
        lli c, d;
        cin >> c >> d;
        visited[i] = false;
        vertexes[i] = {i, 0, c, d, c, 0};
    }

    for (lli i = 0; i < n; i++){
        if (visited[i] == false){
            dfs(i);
        }
    }

    sort(vertexes, vertexes+n, cmp);
    for (int i = 0; i < n; i++){
        indexes[vertexes[i].num] = i;
    }

    for (lli i = 0; i < n; i++){
        for (lli j = 0; j < n; j++) costvisited[j] = false;
        costc = vertexes[i].costc;
        countchild = 0;
        costsum(vertexes[i].num);
        vertexes[i].totalcostc = costc;
        vertexes[i].childscount = countchild;
    }

    cout << letsgoooo(n, m) << "\n";
    return 0;
}