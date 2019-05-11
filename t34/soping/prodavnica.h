#ifndef PRODAVNICA_H_INCLUDED
#define PRODAVNICA_H_INCLUDED

#include <mutex>
#include <condition_variable>
#include <map>

#include "povratna.h"
#include "kupac.h"

using namespace std;

class Prodavnica {
private:
    condition_variable free;
    mutex m;
    int slobodne;
    map<int, int> redni_broj; // rbr:kabina
    Kupac& kupac;
public:
    Prodavnica(Kupac& k, int n): kupac(k), slobodne(n) {
        // Prosiriti po potrebi ...
    }

    // Metoda koju poziva nit koja simulira kupca, kada kupac obavlja soping (procedura je opisana u zaglavlju main.cpp)
    // Metoda je blokirajuca - ako kupac mora da ceka da se oslobodi kabina za probu, izvrsenje ce blokirati dok se za to ne steknu uslovi.
    //
    // rbr - Redni broj kupca
    //
    // Potrebno je pozvati metodu kupac.ceka kada su kabine zauzete i kupac mora da ceka.
    // Potrebno je pozvati metodu kupac.proba_odecu kada kupac udje u kabinu da proba odecu.
    // Potrebno je pozvati metodu kupac.zavrsio kada je kupac zavrsio probu odece.
    povratna_vrednost kupi(int rbr) {
        unique_lock<mutex> lock(m);
        povratna_vrednost ret;
        system_clock::time_point start = system_clock::now();
        while(slobodne == 0){
            kupac.ceka(rbr);
            free.wait(lock);
        }
        ret.cekao_na_kabinu = system_clock::now() - start;
        slobodne--;
        kupac.proba_odecu(rbr);
        lock.unlock();
        this_thread::sleep_for(seconds(1));
        lock.lock();
        ret.kupio = rand()%2;
        kupac.zavrsio(rbr, ret);
        slobodne++;
        free.notify_one();
    }
};

#endif // PRODAVNICA_H_INCLUDED
