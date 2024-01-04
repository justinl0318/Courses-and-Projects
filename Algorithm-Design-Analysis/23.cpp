#include <bits/stdc++.h>
using namespace std;
#define MAXN 400001
#define lli long long int

struct Edgepair{
    lli v; //vertex v from u
    lli length;
    lli cost;
};

struct Mypair{
    lli u; //vertex u
    lli length;
    lli cost;
    lli maxlength;
    Mypair(lli u, lli length, lli cost, lli maxlength) : u(u), length(length), cost(cost), maxlength(maxlength){}
};

struct cmp{
    bool operator()(Mypair const& p1, Mypair const& p2){
         //ascending order according to length, if length same then ascending order according to cost
        if (p1.length > p2.length) return true;
        else if (p1.length == p2.length && p1.cost >= p2.cost) return true;
        return false;
    }
};

vector<Edgepair> adjlist[MAXN];
lli costs[MAXN], lengths[MAXN], parents[MAXN];
bool visited[MAXN];

void prim(lli n){
    
    for (lli i = 0; i < n; ++i){
        costs[i] = INT32_MAX;
        lengths[i] = INT32_MAX;
        visited[i] = false;
    }

    priority_queue<Mypair, vector<Mypair>, cmp> pq;
    pq.push({0, 0, 0, -1});
    costs[0] = 0, lengths[0] = 0;

    while (pq.empty() == false){
        auto minedge = pq.top();
        pq.pop();

        lli u = minedge.u;
        if (visited[u] == true) continue;
        visited[u] = true;
        costs[u] = minedge.cost;
        //printf("below is u\n");
        //printf("u = %d length= %d cost= %d maxlength=%d\n", u, minedge.length, minedge.cost, minedge.maxlength);

        for (auto i = adjlist[u].begin(); i != adjlist[u].end(); ++i){
            lli v = i->v;
            lli length = i->length;
            lli cost = i->cost;

            if (visited[v] == false && length <= lengths[v]){
                lengths[v] = length;
                if (length > minedge.maxlength){
                    pq.push({v, length, cost, length});
                    //printf("11v=%d length=%d cost=%d maxlength=%d\n", v, length, cost, length);
                }
                else if (length == minedge.maxlength){
                    pq.push({v, length, minedge.cost + cost, minedge.maxlength});
                    //printf("22v=%d length=%d cost=%d maxlength=%d\n", v, length, minedge.cost + cost, minedge.maxlength);
                }
                else{
                    pq.push({v, length, minedge.cost, minedge.maxlength});
                    //printf("33v=%d length=%d cost=%d maxlength=%d\n", v, length, minedge.cost, minedge.maxlength);
                }
            }
        }
    }
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
    for (int i = 0; i < m; ++i){
        lli u, v, l, c;
        fastscan(u), fastscan(v), fastscan(l), fastscan(c);
        Edgepair edgepair = {v, l, c};
        adjlist[u].push_back(edgepair);
        Edgepair edgepair2 = {u, l, c};
        adjlist[v].push_back(edgepair2);
    }

    prim(n);
    lli totalcost = 0;
    for (int i = 0; i < n; ++i){
        totalcost += costs[i];
    }
    cout << totalcost << "\n";
    return 0;
}