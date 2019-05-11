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
    int salt; // 0 => zauzeto
    const int salteri = 2;
    int ukupno;

public:
    Posta(Klijent& kl) : klijent(kl), salt(2), ukupno(0) {
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
        while(salt == 0){
            unique_lock<mutex> lock(m);
            klijent.ceka(rbr, svota);
            free.wait(lock);
        }
            salt--;

            klijent.uplacuje(rbr, salt, svota);
            this_thread::sleep_for(seconds(1));

            ukupno += svota;
            klijent.napusta(rbr, salt, ukupno);
            salt++;
            free.notify_all();
    }
};

#endif // POSTA_H_INCLUDED