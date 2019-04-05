#ifndef PARKING_H_INCLUDED
#define PARKING_H_INCLUDED

#include <mutex>
#include <condition_variable>

#include "automobil.h"

using namespace std;

class Parking {
    Automobil& automobil;
    int taken; // 0 => slobodno mesto
    int ulazi;
    mutex m;
    condition_variable free;
public:
    Parking(Automobil& a) : automobil(a), taken(0), ulazi(0) {
        // Prosiriti po potrebi ...
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
        while(taken != 0){
            automobil.ceka(rbr, ulaz);
            free.wait(lock);
        }
        while(ulaz != ulazi){
            automobil.ceka(rbr, ulaz);
            free.wait(lock);
        }
        automobil.zauzima(rbr);
        taken = 1;
        ulazi = ulaz == 2 ? 0 : ++ulaz;// Ne radi lololo
    }

    // Metoda koju poziva nit koja simulira kretanje automobila kada on napusta parking na koji je prethodno usao.
    //
    // rbr  - Redni broj automobila
    //
    // Potrebno je pozvati metodu automobil.napusta kada automobil napusti parking mesto.
    void izadji(int rbr) {
        unique_lock<mutex> lock(m);
        automobil.napusta(rbr);
        taken = 0;
        free.notify_all();
    }
};

#endif // PARKING_H_INCLUDED
