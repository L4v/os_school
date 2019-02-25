/*
Napraviti funkciju:

vector<int> min(const vector<int>& a, const vector<int>& b);

Ova funkcija vraća vektor koji sadrži minimume korespodentnih elemenata vektora a i b.
Npr: r[0]=min(a[0],b[0]), r[1]=...
Podrazumeva se: a.size()==b.size()
*/

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

//typedef vector<string>::const reverse_iterator Vcri;

bool comp(int a, int b){
    return (a < b);
}

vector<int> min(const vector<int>& a, const vector<int>& b){
    vector<int> ret_vector, ta, tb;

    ta = a;
    tb = b;
    auto j = tb.begin();
    for(auto i = ta.begin(); i != ta.end(); i++, j++)
        ret_vector.push_back(*i < *j ? *i : *j);
    return ret_vector;
}

int main()
{
    vector<int> a = {1, 2, 3, 4, 5};
    vector<int> b = {42, 3, 0, 1, 3};
    int x;
    cout << "PRVI VEKTOR:" << endl;
    for(auto i : a)
        cout << i;
    cout << endl << "DRUGI VEKTOR:" << endl;
    for(auto i : b)
        cout << i;
    cout << endl;

    vector<int> minimalni = min(a, b);
    cout << "MINIMUMI: " << endl;
    for(auto i : minimalni)
        cout << i << " ";
    cout << endl;

    return 0;
}
