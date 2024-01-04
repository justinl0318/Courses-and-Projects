#include <bits/stdc++.h>
using namespace std;
#define MAXN 2000
#define lli long long int

typedef pair<lli, int> mypairs; //<distance from 0(root), vertex u>
typedef pair<int, int> edgepair; //<vertex v from u, weight>
vector<edgepair> adjlist[MAXN];
lli sum = 0;
lli minsum = INT64_MAX;

void dijkstra(int n){
    int dist[n];
    bool visited[n];
    for (int i = 0; i < n; i++){
        dist[i] = 100000000;
        visited[i] = false;
    }

    //pq is a minheap according to mypairs.first
    priority_queue<mypairs, vector<mypairs>, greater<mypairs>> pq; 

    pq.push({0, 0}); //we start from vertex 0, distance from vertex 0 to 0 is 0
    dist[0] = 0;
    while (pq.empty() == false){
        int u = pq.top().second;
        pq.pop();

        if (visited[u] == true)continue;
        visited[u] = true;

        for (int i = 0; i < adjlist[u].size(); i++){
            int v = adjlist[u][i].first; //nearby vertex of u
            int weight = adjlist[u][i].second;
            if (dist[u] + weight < dist[v]){
                dist[v] = dist[u] + weight;
                pq.push({dist[v], v});
            }
        }
    }

    for (int i = 0; i < n; i++){
        sum += dist[i];
    }
    if (sum < minsum){
        minsum = sum;
    }
    sum = 0;
}

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    cin >> n >> m;
    for (int i = 0; i < m; i++){
        int a, b, c;
        cin >> a >> b >> c;
        a--, b--;
        adjlist[a].push_back({b, c});
    }
    
    for (int i = 1; i < n; i++){
        adjlist[0].push_back({i, 0});
        dijkstra(n);
        adjlist[0].pop_back();
    }

    cout << minsum;
    return 0;
}