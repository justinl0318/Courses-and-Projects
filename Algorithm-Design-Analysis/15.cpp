#include <bits/stdc++.h>
using namespace std;
#define lli long long int
#define LEN 100001

typedef struct customer{
    lli prep;
    lli finish;
}Customer;

typedef struct group{
    lli gpreptime;
    lli gtotaltime;
    lli len;
}Group;

bool cmp(Customer a, Customer b){
    return a.finish > b.finish;
}

bool cmp2(Group a, Group b){
    lli choice1 = a.gpreptime * b.len;
    lli choice2 = a.len * b.gpreptime;
    return choice1 < choice2;
}

Customer arr[LEN];
Group groupstime[LEN];

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
    lli n;
    fastscan(n);

    for (int j = 0; j < n; j++){
        lli num;
        fastscan(num);

        for (int i = 0; i < num; i++){
            Customer customer;
            fastscan(customer.prep);
            fastscan(customer.finish);
            arr[i] = customer;
        }

        sort(arr, arr+num, cmp);
        lli preptime = 0, currmaxtotaltime = -1;
        for (int i = 0; i < num; i++){
            preptime += arr[i].prep;
            if (preptime + arr[i].finish > currmaxtotaltime){
                currmaxtotaltime = preptime + arr[i].finish;
            }
        }

        Group group;
        group.gpreptime = preptime;
        group.gtotaltime = currmaxtotaltime * num;
        group.len = num;
        groupstime[j] = group;
    }

    sort(groupstime, groupstime+n, cmp2);
    lli totaltime = 0;
    lli sumprep = 0;
    for (lli i = 0; i < n; i++){
        totaltime += groupstime[i].gtotaltime + (sumprep * groupstime[i].len);
        sumprep += groupstime[i].gpreptime;
    }
    cout << totaltime << "\n";
    return 0;
}