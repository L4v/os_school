#ifndef SKLADISTE_H_INCLUDED
#define SKLADISTE_H_INCLUDED

#include <mutex>
#include <condition_variable>
#include <list>

#include "kamion.h"

using namespace std;
using namespace chrono;

class Skladiste {
private:
    mutex m;
    condition_variable free_zap, free_nezap;
    Kamion& kamion;
    int skladista;
    int zap;
public:
    Skladiste(Kamion& k) : kamion(k), skladista(2), zap(0){
        // Prosiriti po potrebi ...
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
        while(skladista == 0){
            kamion.ceka(rbr, kolicina, zapaljivo);
            if(zapaljivo){
                zap++;
                free_zap.wait(lock);
                zap--;
            }else{
                free_nezap.wait(lock);
            }
        }
        skladista--;
        kamion.istovara(rbr, kolicina, zapaljivo, skladista+1);
        lock.unlock();
        this_thread::sleep_for(seconds(kolicina));
        lock.lock();
        skladista++;
        kamion.odlazi(rbr);

        if(zap != 0){
            free_zap.notify_one();
        }else{
            free_nezap.notify_one();
        }

    }
};

#endif // SKLADISTE_H_INCLUDED
