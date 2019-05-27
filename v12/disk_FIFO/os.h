#ifndef OS_H_INCLUDED
#define OS_H_INCLUDED

#include <mutex>
#include <condition_variable>
#include <vector>
#include <deque>

#include "dijagnostika.h"

using namespace std;
using namespace chrono;

class OS {
private:
    struct UI_zahtev{
        condition_variable cv;
        bool ispunjen;
        int staza;
        UI_zahtev(int i):ispunjen(false), staza(i){}
    };

    Dijagnostika& dijagnostika;
    deque<UI_zahtev*> zahtevi;
    mutex m;
    bool obradjuje;
    condition_variable cv_obradjivac;


public:
    OS(Dijagnostika& d) : dijagnostika(d) {
        obradjuje = false;
    }

    Dijagnostika& getDijagnostika() {
        return dijagnostika;
    }

    // Metoda koju poziva nit koja simulira izvršenje procesa, kako bi se uputio zahtev ka U/I uređaju
    //
    // id_procesa - ID procesa koji upućuje zahtev
    // broj_staze - broj staze diska kojoj nit želi da pristupi
    //
    // Metoda treba da formira novi zahtev i uputi ga U/I uređaju na obradu. Proces će biti blokiran dok god se ovaj zahtev ne izvrši. Pre nego što stupi u blokadu, potrebno je pozvati dijagnostika.proces_ceka.
    void uputi_UI_zahtev(int id_procesa, int broj_staze) {
        unique_lock<mutex> lock(m);

        UI_zahtev *zahtev = new UI_zahtev(broj_staze);
        zahtevi.push_front(zahtev);
        cv_obradjivac.notify_one();
        while(!zahtev->ispunjen){
            dijagnostika.proces_ceka(id_procesa, broj_staze);
            zahtev->cv.wait(lock);
        }

    }

    // Metoda koju poziva nit koja simulira deo operativnog sistema koji upravlja diskom, kako bi se obradio jedan pristigli zahtev
    //
    // Prema uslovima zadatka, metoda treba da obradi zahtev koji je najranije pristigao.
    //
    // Povratna vrednost metode treba da bude broj staze koji je obrađen.
    int obradi_zahtev() {
        unique_lock<mutex> lock(m);
        while(zahtevi.empty())
            cv_obradjivac.wait(lock);

        UI_zahtev* zahtev = zahtevi.back();
        zahtevi.pop_back();

        lock.unlock();
        this_thread::sleep_for(milliseconds(300));
        lock.lock();

        zahtev->ispunjen = true;

        zahtev->cv.notify_one();

        return zahtev->staza;

    }
};

#endif // OS_H_INCLUDED
