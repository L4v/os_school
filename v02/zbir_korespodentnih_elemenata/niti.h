#ifndef NITI_H_INCLUDED
#define NITI_H_INCLUDED

#include <vector>
#include <thread>

#include "suma_vektora.h"

using namespace std;

// v - vektor A čije elemente treba sumirati
// v - vektor B čije elemente treba sumirati
// povratna vrednost - suma korespondentnih elemenata vektora A i B, izračunata pozivanjem dve niti
vector<double> sumiraj(vector<double> a, vector<double> b) {
    vector<double> sum(a.size());
    vector<double>::const_iterator polaa = a.begin() + a.size() / 2;
    vector<double>::const_iterator polab = b.begin() + b.size() / 2;
    vector<double>::iterator polasum = sum.begin() + sum.size() / 2;
    thread t1(f, a.begin(), polaa, b.begin(), sum.begin());
    thread t2(f, polaa, a.end(), polab, polasum);
    t1.join(); t2.join();
    return sum;

}

#endif // NITI_H_INCLUDED
