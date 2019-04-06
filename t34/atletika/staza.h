#ifndef STAZA_H_INCLUDED
#define STAZA_H_INCLUDED

#include <mutex>
#include <condition_variable>

#include "povratna.h"
#include "takmicar.h"

using namespace std;
using namespace chrono;

class Atletska_staza {
private:
    Takmicar& takmicar;
    mutex m;
    condition_variable free;
    int taken; // kada 0 => slobodno
public:
    Atletska_staza(Takmicar& tak) : takmicar(tak), taken(0) {
        // Prosiriti po potrebi ...
    }

    // Metoda koju poziva nit koja simulira skakaca kako bi takmicar obavio skok.
    // Ukoliko je zaletiste zauzeto, ova metoda ce stajati blokirana dok se ono ne oslobodi i takmicar obavi nastup.
    //
    // rbr - Redni broj takmicara
    //
    // Potrebno je pozvati metodu takmicar.skakac_ceka kada skakac ne moze da stupi na zaletiste.
    // Potrebno je pozvati metodu takmicar.skakac_skace kada skakac stupi na zaletiste, pre skoka.
    // Potrebno je pozvati metodu takmicar.skakac_zavrsio kada je skakac zavrsio i kada treba prikazati njegov rezultat.
    Povratna_vrednost skaci(int rbr) {
        Povratna_vrednost ret;
        system_clock::time_point start = system_clock::now();
        unique_lock<mutex> lock(m);
        while(taken != 0){
            takmicar.skakac_ceka(rbr);
            free.wait(lock);
        }
        takmicar.skakac_skace(rbr);
        taken++;
        lock.unlock();
        this_thread::sleep_for(seconds(1));
        lock.lock();
        system_clock::time_point end = system_clock::now();
        ret.trajanje = end - start;
        ret.rezultat = rand()%9 + 1;
        taken --;
        takmicar.skakac_zavrsio(rbr, ret);
        free.notify_one();
        return ret;
    }

    // Metoda koju poziva nit koja simulira bacaca kako bi takmicar obavio bacanje.
    // Ukoliko je zaletiste zauzeto, ova metoda ce stajati blokirana dok se ono ne oslobodi i takmicar obavi nastup.
    //
    // rbr - Redni broj takmicara
    //
    // Potrebno je pozvati metodu takmicar.bacac_ceka kada bacac ne moze da stupi na zaletiste.
    // Potrebno je pozvati metodu takmicar.bacac_skace kada bacac stupi na zaletiste, pre skoka.
    // Potrebno je pozvati metodu takmicar.bacac_zavrsio kada je bacac zavrsio i kada treba prikazati njegov rezultat.
    Povratna_vrednost baciKoplje(int rbr) {
        system_clock::time_point start = system_clock::now();
        Povratna_vrednost ret;
        unique_lock<mutex> lock(m);
        while(taken != 0){
            takmicar.bacac_ceka(rbr);
            free.wait(lock);
        }
        takmicar.bacac_baca(rbr);
        taken++;
        lock.unlock();
        this_thread::sleep_for(seconds(1));
        lock.lock();
        system_clock::time_point end = system_clock::now();
        ret.trajanje = end - start;
        ret.rezultat = rand()%100 + 1;
        taken --;
        takmicar.bacac_zavrsio(rbr, ret);
        free.notify_one();
        return ret;
    }
};

#endif // STAZA_H_INCLUDED
