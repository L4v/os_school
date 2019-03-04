#ifndef NITI_H_INCLUDED
#define NITI_H_INCLUDED

#include <vector>
#include <thread>

#include "suma_vektora.h"

using namespace std;

// v - vektor čije elemente treba sumirati
// povratna vrednost - suma svih elemenata vektora, izračunata pokretanjem 2 niti (svaka treba da obradi jednu polovinu elemenata)
double sumiraj(vector<double> v) {
    double s1, s2;
    vector<double>::const_iterator polovina = v.begin() + v.size() / 2;
    thread t1(f, v.begin(), polovina, ref(s1)),
           t2(f, polovina, v.end(), ref(s2));
    t1.join(); t2.join();
    return s1 + s2;
}

#endif // NITI_H_INCLUDED
