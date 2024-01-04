#include <bits/stdc++.h>
using namespace std;
#define MAXN 200001
#define MAXM 500001
#define lli long long int

vector<int> v[MAXN];
vector<int> reversev[MAXN];
stack<int> st;
bool visited[MAXN] = {false};
vector<int> scc[MAXN];

int n, m;
lli ans = 0;
lli components = 1;

void dfs1(int x){
    visited[x] = true;
    for (int i = 0; i < v[x].size(); i++){
        if (visited[v[x][i]] == false){
            dfs1(v[x][i]);
        }
    }
    st.push(x);
}

void dfs2(int x){
    visited[x] = true;
    for (int i = 0; i < reversev[x].size(); i++){
        if (visited[reversev[x][i]] == false){
            components++;
            dfs2(reversev[x][i]);
        }
    }
}

void korasaju(){
    for (int i = 0; i < n; i++){
        if (visited[i] == false){
            dfs1(i);
        }
    }

    for (int i = 0; i < n; i++){
        visited[i] = false;
    }

    while (st.empty() == false){
        int stacktop = st.top();
        st.pop();
        if (visited[stacktop] == false){
            components = 1;
            dfs2(stacktop);
            ans += (components * (components-1)) / 2;
        }
    }
}

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> n >> m;
    for (int i = 0; i < m; i++){
        int start, end;
        cin >> start >> end;
        start--, end--;
        v[start].push_back(end);
        reversev[end].push_back(start);
    }
    korasaju();
    cout << ans << "\n";
    return 0;
}