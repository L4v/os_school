#ifndef PARKING_H_INCLUDED
#define PARKING_H_INCLUDED

#include <mutex>
#include <condition_variable>

#include "automobil.h"

using namespace std;

class Parking {
private:
    Automobil& automobil;
    enum Stanje{ FREE, TAKEN };
    Stanje stanje;
    condition_variable free;
    mutex m;
public:
    Parking(Automobil& a) : automobil(a), stanje(FREE) {
        // Proširiti po potrebi ...
    }

    // Metoda koju poziva nit koja simulira kretanje automobila kako bi automobil pokušao da se parkira.
    // Ako je parking zauzet, izvršenje ove metode će trajati dok se parking ne oslobodi i auto se parkira.
    //
    // rbr - Redni broj automobila
    //
    // Potrebno je pozvati metodu automobil.ceka kada je parking zauzet i auto mora da čeka.
    // Potrebno je pozvati metodu automobil.parkira kada auto uspe da se parkira.
    void udji(int rbr) {
        unique_lock<mutex> l(m);
        while(stanje == TAKEN){ // Zbog laznog budjenja od strane OS-a, koristiti while
            automobil.ceka(rbr);
            free.wait(l);
        }
        stanje = TAKEN;
        automobil.parkira(rbr);
    }

    // Metoda koju poziva nit koja simulira kretanje automobila kada auto izlazi sa parkinga (nakon što je bio parkiran).
    //
    // rbr - Redni broj automobila
    //
    // Potrebno je pozvati metodu automobil.napusta kada auto napušta parking mesto.
    void izadji(int rbr) {
        unique_lock<mutex> l(m);
        stanje = FREE;
        automobil.napusta(rbr);
        free.notify_one();
    }
};

#endif // PARKING_H_INCLUDED
