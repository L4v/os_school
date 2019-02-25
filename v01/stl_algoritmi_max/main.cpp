/*
Korisnik treba da unese 5 celih brojeva koristeći tastaturu (standardni ulaz).
Korišćenjem STL algoritma, među tih 5 brojeva potrebno je pronaći najveći.
*/

#include <iostream>
#include <list>

using namespace std;

int main()
{
    int x;
    list<int> brojevi;

    for(int i = 0; i < 5; i ++){
        cin >> x;
        brojevi.push_back(x);
    }

    brojevi.sort();
    cout << "Max broj je: " << brojevi.back();

    return 0;
}
