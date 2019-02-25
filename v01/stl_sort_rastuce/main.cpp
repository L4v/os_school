/*
Uneti 5 celih brojeva sa standardnog ulaza.
Korišćenjem STL algoritma sortirati brojeve u opadajućem redosledu.
*/

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

bool comp(int i, int j){
    return(i < j);
}

int main()
{
    vector<int> brojevi;
    int x;
    for(int i = 0; i < 5; i ++){
        cin >> x;
        brojevi.push_back(x);
    }

    sort(brojevi.begin(), brojevi.end(), comp);
    for(auto i : brojevi)
        cout << i;
    cout << endl;

    return 0;
}
