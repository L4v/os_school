#ifndef SKLADISTE_H_INCLUDED
#define SKLADISTE_H_INCLUDED

#include <mutex>
#include <condition_variable>

#include "kamion.h"

using namespace std;
using namespace chrono;

class Skladiste {
private:
    Kamion& kamion;
    mutex m;
    condition_variable* free; // free[0] zapaljivo, free[1] obicno
    int zapaljiva_roba; // koliko kamiona sa zapaljivom robom ceka
    bool* stanje;
public:
    Skladiste(Kamion& k) : kamion(k), zapaljiva_roba(0){
        this->free = new condition_variable[2];
        this->stanje = new bool[2];
        stanje[0] = false; stanje[1] = false;
    }

    // Metoda koju poziva nit koja simulira kretanje kamiona kada on pokusava da istovari robu.
    // Metoda je blokirajuca - ako su sve rampe za istovar zauzete, izvrsenje ce blokirati dok se neka ne oslobodi.
    //
    // rbr       - Redni broj kamiona
    // kolicina  - Kolicina robe koja se prevozi (u tonama)
    // zapaljivo - Ukazuje na to da li je roba koja se prevozi zapaljiva (takva roba ima prioritet pri istovaru!)
    //
    // Potrebno je pozvati metodu kamion.ceka kada su rampe zauzete i kamion mora da ceka.
    // Potrebno je pozvati metodu kamion.istovara kada zapocne istovar robe iz kamiona (primetiti da ta metoda zahteva da joj se prosledi indeks rampe na kojoj se obavlja istovar!)
    // Potrebno je pozvati metodu kamion.odlazi kada je kamion zavrsio istovar i odlazi.
    void istovari(int rbr, int kolicina, bool zapaljivo) {
        unique_lock<mutex> lock(m);
        while(stanje[0] && stanje[1]){
            kamion.ceka(rbr, kolicina, zapaljivo);
            if(zapaljivo){
                zapaljiva_roba++;
                free[0].wait(lock);
                zapaljiva_roba--;
            }else{
                free[1].wait(lock);
            }
        }
        int id_rampe = stanje[0] ? 1 : 0;
        stanje[id_rampe] = true;
        kamion.istovara(rbr, kolicina, zapaljivo, id_rampe);
        lock.unlock();
        this_thread::sleep_for(seconds(kolicina));
        lock.lock();
        kamion.odlazi(rbr);
        stanje[id_rampe] = false;
        if(zapaljiva_roba) free[0].notify_one();
        else free[1].notify_one();
    }
};

#endif // SKLADISTE_H_INCLUDED
