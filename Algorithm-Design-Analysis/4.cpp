#include <bits/stdc++.h>
using namespace std;
#define lli long long int

class Node{
public:
    lli freq;
    bool visited;
    Node *left;
    Node *right;
    Node *up;
    Node (lli frequency){
        freq = frequency;
        left = right = NULL;
        up = NULL;
        visited = false;
    }
};

class Compare{
public:
    bool operator()(Node* a, Node* b){
        return a->freq > b->freq;
    }
};

priority_queue<Node*, vector<Node*>, Compare> pq; //min heap;
vector<Node*> v;

Node *generate(){
    while (pq.size() != 1){
        Node *left = pq.top();
        pq.pop();
        Node *right = pq.top();
        pq.pop();
        Node *newnode = new Node(left->freq+right->freq);
        newnode->left = left;
        newnode->right = right;
        left->up = newnode;
        right->up = newnode;
        pq.push(newnode);
    }
    return pq.top();
}

lli path[10000];


void print(Node *leaf, int index){
    while (leaf->up != NULL){
        if (leaf == leaf->up->left){
            path[index] = 0;
        }
        else{
            path[index] = 1;
        }
        cout << leaf->freq;
        leaf = leaf->up;
    }
    for (int i = 0; i < index; i++){
        cout << path[i];
    }
    cout << endl;
}



int main(){
    lli n;
    cin >> n;
    lli arr[n];
    for (lli i = 0; i < n; i++){
        lli num;
        cin >> num;
        Node *newnode = new Node(num);
        pq.push(newnode);
        v.push_back(newnode);
        arr[i] = num;
    }

    if (n == 1){
        printf("0\n");
        return 0;
    }

    Node *root = generate();
    for (int i = 0; i < n; i++){
        Node *leaf = v[i];
        int index = 0;
        
        while (leaf->up != NULL){
            if (leaf == leaf->up->left){
                path[index] = 0;
            }
            else{
                path[index] = 1;
            }
            leaf = leaf->up;
            index++;
        }
        for (int i = index-1; i >= 0; i--){
            cout << path[i];
        }
        cout << endl;

    }
    return 0;
}



