/*
Napraviti funkciju:

vector<int> min_n(const vector<int>& v,	int n);

Ova funkcija vraća vektor koji sadrži n najmanjih elemenata iz vektora v.
Podrazumeva se: v.size()>=n
*/

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

bool comp(int x, int y){
    return x < y;
}

vector<int> min_n(const vector<int>& v,	int n){
    vector<int> tmp = v, neue;
    sort(tmp.begin(), tmp.begin() + n, comp);
    for(auto i = tmp.begin(); i != tmp.begin() + n; i ++)
        neue.push_back(*i);
    return neue;
}

int main()
{
    vector<int> v = {10, 2, 1, 0, 42, 32, 16, 64, 3};
    cout << "Prva 3 min elem: " << endl;
    for(auto i : min_n(v, 3))
        cout << i << " ";
    cout << endl;
    return 0;
}
