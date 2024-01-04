#include <bits/stdc++.h>
using namespace std;

const int MAXN = 500000;
const int MAXC = 100000;
const int INF = 100000;
int N;
int a[MAXN +1];
long long ans;
vector <int > aR(MAXC *2) , amR(MAXC *2);

void solve (int L, int R) {
    if (L == R) return ;
    int mid = (L + R) / 2;
    solve(L, mid);
    solve(mid +1, R);
    int minL , minR;
    // Solve a_l > a_r
    minR = INF;
    vector <int > undo;
    // use vector with undo instead of map
    for (int i = mid +1; i <= R; i++) {
        minR = min(minR , a[i]);
        amR[a[i] + minR ]++; undo. push_back (a[i] + minR);
    }
    int curR = mid +1;
    minL = minR = INF;
    for (int i = mid; i >= L; i--) {
    minL = min(minL , a[i]);
    while (curR <= R && minL <= a[curR ]) {
        minR = min(minR , a[curR ]);
        amR[a[curR] + minR ]--; undo. push_back (a[curR] + minR);
        aR[a[curR ]]++; undo. push_back (a[curR ]);
        curR ++;
    }
    ans += aR[a[i] - minL] + amR[a[i]];
    }
    for (int i : undo) aR[i] = amR[i] = 0;
    undo.clear ();
}

signed main (){
    cin >> N;
    for (int i = 1; i <= N; i++) {
        cin >> a[i];
    }
    // assume a_l > a_r
    solve (1, N);
    // change it to a_r > a_l
    for (int i = 1; i <= N/2; i++) {
        swap(a[i], a[N+1-i]);
    }
    solve (1, N);
    cout << ans << '\n';
}