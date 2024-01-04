#include <bits/stdc++.h>
using namespace std;
#define lli long long int
#define LEN 5000002
#define SIZE 1048575
lli segmenttree[SIZE] = {0};

int cmid(int left, int right){
    return (left + right)/2;
}

int buildtree(int left, int right, int index){
    if (left == right){
        segmenttree[index] = 0;
        return 0;
    }
    int mid = cmid(left, right);
    segmenttree[index] = buildtree(left, mid, index*2+1) + buildtree(mid+1, right, index*2+2);
    return segmenttree[index];
}

void update(int ss, int se, int index, int root){
    if (index < ss || index > se) return;
    segmenttree[root]++;
    if (se != ss){
        int mid = cmid(ss, se);
        update(ss, mid, index, root*2+1);
        update(mid+1, se, index, root*2+2);
    }
}

int query(int ss, int se, int indexstart, int indexend, int root){
    if (indexstart <= ss && indexend >= se) return segmenttree[root];
    if (se < indexstart || ss > indexend) return 0;
    int mid = cmid(ss, se);
    return query(ss, mid, indexstart, indexend, root*2+1) + query(mid+1, se, indexstart, indexend, root*2+2);
}

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    string s;
    cin >> s;
    lli n = s.length();
    lli k;
    cin >> k;
    if (k == (n*(n-1)/2)){
        sort(s.begin(), s.end());
        cout << s << "\n";
        return 0;
    }

    vector<queue<int>> v(62);
    for (int i = 0; i < n; i++){
        int ascii = s[i];
        int index;
        //printf("%d\n", ascii);
        if (ascii >= 48 && ascii <= 59){
            index = s[i]-'0';
        }
        else if (ascii >= 65 && ascii <= 90){
            index = s[i]-'A'+10;
        }
        else if (ascii >= 97 && ascii <= 122){
            index = s[i]-'a'+10+26;
        }
        //printf("i = %d, %d\n", i, index);
        v[index].push(i);
    }

    vector<int> used(n+1);
    string finals = "";
    int length = 0;
    int prevpos, prevquery;
    while (k > 0 && length < n){
        prevpos = n+1;
        for (int d = 0; d < 62; d++){

            if (v[d].empty() == true){
                continue;
            }

            int pos = v[d].front();
            if (pos > prevpos) continue;
            int cost = pos - query(0, n-1, 0, pos-1, 0);
            if (k < cost){
                prevpos = pos;
                continue;
            }
            k -= cost;

            //printf("k = %d\n", k);
            if (d >= 0 && d <= 9)
                printf("%c", '0'+d);
            else if (d >= 10 && d <= 35)
                printf("%c", 'A'+(d-10));
            else if (d >= 36 && d <= 61)
                printf("%c", 'a'+(d-36));
            //finals.push_back((char)('0'+d));
            //printf("\n");
            length++;
            update(0, n-1, pos, 0);
            used[pos] = true;
            v[d].pop();
            break;
        }
    }

    if (length == n){
        printf("\n");
        return 0;
    }

    for (int i = 0; i < n; i++){
        if (used[i] == false){
            printf("%c", s[i]);
        }
    }
    printf("\n");

    return 0;
}