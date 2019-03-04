/*
Napraviti funkciju:

vector<int> min_n(const vector<int>& v,	int n);

Ova funkcija vraća vektor koji sadrži n najmanjih elemenata iz vektora v.
Podrazumeva se: v.size()>=n
*/

#include <iostream>
#include <vector>

using namespace std;

vector<int> min(const vector<int>& a, const vector<int>& b, int n){
    vector<int> tmpa = a, tmpb = b, rez;
    vector<int>::iterator ita, itb;
    for(ita = tmpa.begin(), itb = tmpb.begin(); ita != tmpa.begin() + n; ita++, itb ++)
        rez.push_back(*ita < *itb ? *ita : *itb);
    return rez;
}

int main()
{
    vector<int> a = {1, 2, 3, 4, 5},
                b = {0, 3, 4, 5, 6},
                mins;
    mins = min(a, b, 3);
    cout << "Novi vektori: ";
    for(auto i : mins) cout << i << " ";
    cout << endl;
    return 0;
}
