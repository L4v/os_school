#ifndef NITI_H
#define NITI_H

#include <thread>
#include <vector>

using namespace std;

// Implementacija sabiranja korespondentnih elemenata celih vektora A i B pokretanjem onoliko niti koliko ima elemenata u vektorima
//
// a - Vektor A
// b - Vektor B
//
// Rezultat izvršavanja treba da bude vector<int> koji sadrži rezultat sabiranja korespondentnih elemenata.
//
vector<int> izracunaj(vector<int>& a, vector<int>& b) {
    vector<int> c(a.size());
    vector<int>::const_iterator ita, itb;
    vector<int>::iterator it;
    thread threads[a.size()];
    int i = 0;

    for(i = 0, ita = a.begin(), itb = b.begin(), it = c.begin(); ita != a.end(); ita++, itb++, it++, i ++)
        threads[i] = thread(saberiPar, ita, itb, it);

    for(i = 0; i < a.size(); i ++)
        threads[i].join();

    return c;

}

#endif // NITI_H
