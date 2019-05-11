#ifndef TRZNI_CENTAR_H_INCLUDED
#define TRZNI_CENTAR_H_INCLUDED

#include <mutex>
#include <condition_variable>
#include <list>

#include "kupac.h"

using namespace std;

class Trzni_centar {
private:
    mutex m;
    condition_variable * free; // uslovna promenljiva za svaku kasu
    int* red; // Duzina reda na kasi
    bool* stanje; // false-slobodno | true-zauzeto
    Kupac& kupac;
public:
    Trzni_centar(Kupac& k) : kupac(k){
        this->free = new condition_variable[2];
        this->red = new int[2];
        this->red[0] = 0; this->red[1] = 0;
        this->stanje = new bool[2];
        this->stanje[0] = false; this->stanje[1] = false;
    }

    // Metoda koju poziva nit koja simulira kupca kako bi se obavila kupovina.
    // Metoda je blokirajuca - ako su sve kase zauzete, onda se izvrsenje blokira dok se neka ne oslobodi.
    //
    // rbr           - Redni broj kupca
    // broj_artikala - Broj artikala koje kupac zeli da kupi
    //
    // Potrebno je pozvati metodu kupac.ceka kada je izabrana kasa zauzeta i kupac mora da ceka.
    // Potrebno je pozvati metodu kupac.kupuje kada se kasa oslobodi i kupac pocinje kupovinu.
    // Potrebno je pozvati metodu kupac.zavrsio kada je kupac zavrsio kupovinu.
    int kupi(int rbr, int broj_artikala) {
        unique_lock<mutex> lock(m);
        int kasa = red[0] <= red[1] ? 0 : 1; // Na koju kasu ide kupac
        while(stanje[kasa]){
            red[kasa]++;
            kupac.ceka(rbr, kasa+1);
            free[kasa].wait(lock);
            red[kasa]--;
        }
        stanje[kasa] = true;
        kupac.kupuje(rbr, kasa+1, broj_artikala);
        lock.unlock();
        this_thread::sleep_for(chrono::seconds(broj_artikala));
        lock.lock();
        kupac.zavrsio(rbr);
        stanje[kasa] = false;
        free[kasa].notify_one();
        return kasa+1;
    }
};


#endif // TRZNI_CENTAR_H_INCLUDED
