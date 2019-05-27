#ifndef OS_H_INCLUDED
#define OS_H_INCLUDED

#include <mutex>
#include <condition_variable>
#include <list>

#include "dijagnostika.h"

using namespace std;

class OS {
private:
    struct UI_zahtev{
        int staza;
        bool izvrsen;
        condition_variable cv;
        UI_zahtev(int i):izvrsen(false), staza(i){}
    };

    Dijagnostika& dijagnostika;
    int trenutna_pozicija;
    list<UI_zahtev*> zahtevi;
    condition_variable cv_obrada;
    mutex m;


    int nadji_min(int poz){
        UI_zahtev* min_poz = zahtevi.front();

        for(auto it = zahtevi.begin(); it != zahtevi.end(); it++)
            min_poz = abs((*it)->staza - poz) < abs(min_poz->staza - poz) ? *it : min_poz;

        return min_poz->staza;
    }

public:
    OS(Dijagnostika& d, int t) : dijagnostika(d) {
        trenutna_pozicija = t;
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

        UI_zahtev* zahtev = new UI_zahtev(broj_staze);
        zahtevi.push_back(zahtev);

        cv_obrada.notify_one();

        while(!zahtev->izvrsen){
            dijagnostika.proces_ceka(id_procesa, broj_staze);
            zahtev->cv.wait(lock);
        }

        delete zahtev;

    }

    // Metoda koju poziva nit koja simulira deo operativnog sistema koji upravlja diskom, kako bi se obradio jedan pristigli zahtev
    //
    // Prema uslovima zadatka, metoda treba da obradi zahtev koji je najranije pristigao.
    //
    // Povratna vrednost metode treba da bude broj staze koji je obrađen.
    int obradi_zahtev() {
        unique_lock<mutex> lock(m);
        while(zahtevi.empty())
            cv_obrada.wait(lock);

        trenutna_pozicija = nadji_min(trenutna_pozicija);
        UI_zahtev* zahtev;
        for(auto it = zahtevi.begin(); it != zahtevi.end(); it++)
            if((*it)->staza == trenutna_pozicija){
                zahtev = *it;
                zahtevi.erase(it);
                break;
            }

        lock.unlock();
        this_thread::sleep_for(chrono::milliseconds(300));
        lock.lock();

        zahtev->izvrsen = true;
        zahtev->cv.notify_one();

        return zahtev->staza;

    }
};

#endif // OS_H_INCLUDED
