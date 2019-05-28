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
    // Struktura koja predstavlja jedan zahtev
    struct UI_zahtev{
        condition_variable cv; // Uslovna promenljiva zahteva
        bool ispunjen; // Da li je zahtev ispunjen
        int staza; // Staza na koju treba da ode glava HDD-a
        UI_zahtev(int i):ispunjen(false), staza(i){}
    };

    Dijagnostika& dijagnostika;
    deque<UI_zahtev*> zahtevi; // Red zahteva
    mutex m;
    condition_variable cv_obradjivac; // Uslovna promenljiva obradjivaca zahteva


public:
    OS(Dijagnostika& d) : dijagnostika(d) {
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

        // Kreira se novi zahtev i stavlja na pocetak reda
        UI_zahtev *zahtev = new UI_zahtev(broj_staze);
        zahtevi.push_front(zahtev);

        // Obavestava se obradjivac da postoji novi zahtev
        cv_obradjivac.notify_one();

        // Dok se ne ispuni zahtev, cekamo
        while(!zahtev->ispunjen){
            dijagnostika.proces_ceka(id_procesa, broj_staze);
            zahtev->cv.wait(lock);
        }

        delete zahtev; // Brise zahtev
    }

    // Metoda koju poziva nit koja simulira deo operativnog sistema koji upravlja diskom, kako bi se obradio jedan pristigli zahtev
    //
    // Prema uslovima zadatka, metoda treba da obradi zahtev koji je najranije pristigao.
    //
    // Povratna vrednost metode treba da bude broj staze koji je obrađen.
    int obradi_zahtev() {
        unique_lock<mutex> lock(m);

        // Dokle god ne postoje zahtevi, obradjivac ceka
        while(zahtevi.empty())
            cv_obradjivac.wait(lock);

        // Uzima zahtev sa kraja reda, na kraju se nalazi prvi zahtev
        UI_zahtev* zahtev = zahtevi.back();
        zahtevi.pop_back(); // Izbacuje ga iz reda

        lock.unlock();
        this_thread::sleep_for(milliseconds(300)); // Izvrsava se zahtev
        lock.lock();

        // Obavestava da je zahtev izvrsen
        zahtev->ispunjen = true;
        zahtev->cv.notify_one();

        return zahtev->staza; // Vraca stazu

    }
};

#endif // OS_H_INCLUDED
