/*
Napraviti funkciju:

vector<int> min(const vector<int>& a, const vector<int>& b);

Ova funkcija vraća vektor koji sadrži minimume korespodentnih elemenata vektora a i b.
Npr: r[0]=min(a[0],b[0]), r[1]=...
Podrazumeva se: a.size()==b.size()
*/

#include <iostream>
#include <vector>

using namespace std;

vector<int> min(const vector<int>& a, const vector<int>& b){
    vector<int> tmpa = a, tmpb = b, rez;
    vector<int>::iterator ita, itb;
    for(ita = tmpa.begin(), itb = tmpb.begin(); ita != tmpa.end(); ita++, itb ++)
        rez.push_back(*ita < *itb ? *ita : *itb);
    return rez;
}

int main()
{
    vector<int> a = {1, 2, 3, 4, 5},
                b = {0, 3, 4, 5, 6},
                mins;
    mins = min(a, b);
    cout << "Novi vektori: ";
    for(auto i : mins) cout << i << " ";
    cout << endl;
    return 0;
}
