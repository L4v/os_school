#ifndef NITI_H_INCLUDED
#define NITI_H_INCLUDED

#include <vector>
#include <thread>
#include <list>
#include <iostream>

#include "suma_vektora.h"

using namespace std;

// v - vektor čije elemente treba sumirati
// n - koliko niti treba pokrenuti
// povratna vrednost - suma svih elemenata vektora, izračunata pokretanjem n niti (svaka nit treba da obradi jedan deo elemenata)
double sumiraj(vector<double> v, int n) {
    vector<double>::const_iterator it = v.begin();
    thread t[n];
    double sums[n];
    double sum = 0;
    for(int i = 0; i < n; i ++){
        if(i == n - 1)
            t[i] = thread(f, it, v.end(), ref(sums[n - 1]));
        else{
        t[i] = thread(f, it, it + v.size() / n, ref(sums[i]));
        it += v.size()/n;
        }
    }
    for(int i = 0; i < n; i ++)
        t[i].join();

    for(double i : sums)
        sum += i;

    return sum;

}

#endif // NITI_H_INCLUDED
