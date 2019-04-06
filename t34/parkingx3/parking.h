#ifndef PARKING_H_INCLUDED
#define PARKING_H_INCLUDED

#include <mutex>
#include <condition_variable>
#include <map>

#include "automobil.h"

using namespace std;

class Parking {
    Automobil& automobil;
    bool zauzet; // false-slobodno | true-zauzeto
    int ulaz_id; // trenutni ulaz: {0, 1, 2}
    map<int, int> mesto_id; // automobil:mesto
    mutex m;
    condition_variable free;
public:
    Parking(Automobil& a) : automobil(a), zauzet(false), ulaz_id(0) {
        //this->free = new condition_variable[3];
    }

    // Metoda koju poziva nit koja simulira kretanje automobila kada on pokusava da udje na parking.
    // Metoda je blokirajuca - ukoliko nema slobodnog mesta, izvrsenje je u blokadi dok se ono ne oslobodi.
    //
    // rbr  - Redni broj automobila
    // ulaz - Ulaz na koji automobil pokusava da udje
    //
    // Potrebno je pozvati metodu automobil.ceka kada automobil ne moze da pridje parkingu i mora da ceka.
    // Potrebno je pozvati metodu automobil.zauzima kada automobil zauzme parking mesto.
    void udji(int rbr, int ulaz) {
        unique_lock<mutex> lock(m);
        mesto_id[rbr] = ulaz;
        while(zauzet || ulaz != ulaz_id){
            automobil.ceka(rbr, ulaz);
            free.wait(lock);
        }
        zauzet = true;
        automobil.zauzima(rbr);
        ulaz_id = ulaz_id == 2 ? 0 : ++ulaz_id;
    }

    // Metoda koju poziva nit koja simulira kretanje automobila kada on napusta parking na koji je prethodno usao.
    //
    // rbr  - Redni broj automobila
    //
    // Potrebno je pozvati metodu automobil.napusta kada automobil napusti parking mesto.
    void izadji(int rbr) {
        unique_lock<mutex> lock(m);
        automobil.napusta(rbr);
        zauzet = false;
        free.notify_all();
    }
};

#endif // PARKING_H_INCLUDED
