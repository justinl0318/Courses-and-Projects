#include <bits/stdc++.h>
using namespace std;
#define lli long long int
#define MAXN 1000001

struct Edgepair{
    lli u;
    lli weight;
    Edgepair (lli u, lli weight) : u(u), weight(weight){}
};

struct cmp{
    bool operator()(Edgepair const& a, Edgepair const& b){
        if (a.weight > b.weight) return true;
        return false;
    }
};

vector<Edgepair> adjlist[MAXN];
bool visited[MAXN];
lli weights[MAXN];

void prim(lli n){
    for (int i = 0; i < n; i++){
        visited[i] = false, weights[i] = INT32_MAX;
    }

    priority_queue<Edgepair, vector<Edgepair>, cmp> pq;
    weights[0] = 0;
    pq.push({0, 0}); //start with vertex 0

    while (pq.empty() == false){
        auto minedge = pq.top();
        pq.pop();

        int u = minedge.u;
        if (visited[u] == true) continue;
        visited[u] = true;

        for (auto i = adjlist[u].begin(); i != adjlist[u].end(); i++){
            int v = i->u;
            int weight = i->weight;
            if (visited[v] == false && weight < weights[v]){
                weights[v] = weight;
                pq.push({v, weight});
            }
        }
    }
    lli sum = 0;
    for (int i = 0; i < n; i++){
        sum += weights[i];
    }
    cout << sum << "\n";
    return;
}

void fastscan(lli &number){
    number = 0;
    register lli c;
    c = getchar();
    for (; (c>47 && c<58); c=getchar())
        number = number *10 + c - 48;
}

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    lli n, m;
    fastscan(n), fastscan(m);
    for (int i = 0; i < m; i++){
        lli a, b, c;
        fastscan(a), fastscan(b), fastscan(c);
        a--, b--;
        adjlist[a].push_back({b, c});
        adjlist[b].push_back({a, c});
    }
    prim(n);
    return 0;
}