#ifndef POSTA_H_INCLUDED
#define POSTA_H_INCLUDED

#include <mutex>
#include <condition_variable>

#include "povratna.h"
#include "klijent.h"

using namespace std;
using namespace chrono;

class Posta {
private:
    Klijent& klijent;
    condition_variable free;
    mutex m;
    bool* salter; // true => zauzeto
    int ukupno;

public:
    Posta(Klijent& kl) : klijent(kl), ukupno(0) {
        this->salter = new bool[2];
        this->salter[0] = false; this->salter[1] = false;
    }

    // Metoda koju poziva nit koja simulira klijenta kada on uplacuje neki iznos.
    // Metoda je blokirajuca - ako je salter zauzet, metoda ce stajati blokirana dok se on ne oslobodi.
    //
    // rbr   - Redni broj klijenta
    // svota - Svota koju klijent zeli da uplati
    //
    // Potrebno je pozvati metodu klijent.ceka kada su salteri zauzeti i klijent mora da ceka.
    // Potrebno je pozvati metodu klijent.uplacuje kada klijent stupi na salter i vrsi svoje placanje.
    // Potrebno je pozvati metodu klijent.napusta kada klijent zavrsi placanje i napusta salter.
    void uplati(int rbr, int svota) {
        unique_lock<mutex> lock(m);
        while(salter[0] && salter[1]){
            klijent.ceka(rbr, svota);
            free.wait(lock);
        }
        int id_saltera = salter[0] ? 1 : 0;
        salter[id_saltera] = true;
        klijent.uplacuje(rbr, id_saltera+1, svota);
        lock.unlock();
        this_thread::sleep_for(seconds(svota));
        lock.lock();
        ukupno += svota;
        klijent.napusta(rbr, id_saltera+1, svota);
        salter[id_saltera] = false;
        free.notify_one();
    }
};

#endif // POSTA_H_INCLUDED
