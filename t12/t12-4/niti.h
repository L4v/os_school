#ifndef NITI_H
#define NITI_H

#include <thread>
#include <vector>

using namespace std;

// Implementacija pokretanja tri niti koje treba da pretraže (svaka svoju trećinu) ulazni vektor brojeva i nađu element najbliži nuli
//
// ulaz - Ulazni vektor brojeva koji se pretražuje
//
// Povratna vrednost je nađeni element, najbliži nuli od svih elemenata koji su obuhvaćeni pretragom
//
double izracunaj(vector<double> ulaz) {
    vector<double>::const_iterator it = ulaz.begin();
    int n = ulaz.size() / 3;
    double mins[n], rez;
    thread t[n];
    for(int i = 0; i < 3; i++){
        if(i == n - 1){
            t[i] = thread(pretrazi, it, ulaz.end(), ref(mins[i]));
            continue;
        }
        t[i] = thread(pretrazi, it, it + n, ref(mins[i]));
        it += n;
    }
    for(int i = 0; i < 3; i ++)
        t[i].join();
    rez = mins[0];
    for(int i = 0; i < 3; i ++)
        if(abs(rez) > abs(mins[i]))
            rez = mins[i];
    return rez;
}


#endif // NITI_H
