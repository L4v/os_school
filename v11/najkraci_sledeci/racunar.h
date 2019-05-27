#ifndef RACUNAR_H_INCLUDED
#define RACUNAR_H_INCLUDED

#include <condition_variable>
#include <mutex>
#include <deque>
#include <algorithm>

#include "dijagnostika.h"

using namespace std;

class Racunar {
private:
    struct st_proces{
        int id_procesa, trajanje;
        condition_variable cv;
        st_proces(int i, int j):id_procesa(i), trajanje(j){}
    };

    Dijagnostika& dijagnostika;
    mutex m;
    condition_variable cv_proc, cv_rasp;
    deque<st_proces*> procesi;
    bool rasporedjuje;
    bool cpu_zauzet;

    void dodaj_u_red(st_proces* p, unique_lock<mutex>& lock){
        auto it = find(procesi.begin(), procesi.end(), p);
        if(it == procesi.end())
            procesi.push_back(p);
        p->cv.wait(lock);
    }

public:
    Racunar(Dijagnostika& d) : dijagnostika(d) {
        cpu_zauzet = false;
        rasporedjuje = false;
    }

    Dijagnostika& getDijagnostika() {
        return dijagnostika;
    }

    // Metoda koju poziva nit koja simulira izvršenje procesa, kako bi se proces izvršio
    //
    // id_procesa   - ID procesa
    // trajanje     - trajanje procesa (izraženo u stotinama milisekundi)
    //
    // Ukoliko je procesor već zauzet i ne mogu se izvršavati naredbe procesa, potrebno je pozvati metodu dijagnostika.proces_ceka a nakon toga proces treba da pređe u stanje čekanja.
    // Nakon što se proces izvršio, potrebno je pozvati dijagnostika.proces_zavrsio.
    void izvrsi(int id_procesa, int trajanje) {
        unique_lock<mutex> lock(m);

        while(cpu_zauzet || rasporedjuje){
            dijagnostika.proces_ceka(id_procesa);
            st_proces* p = new st_proces(id_procesa, trajanje);
            dodaj_u_red(p, lock);
            //delete p;
            cv_proc.wait(lock);
        }
        cpu_zauzet = true;

        dijagnostika.proces_kreiran(id_procesa, trajanje);

        this_thread::sleep_for(chrono::milliseconds(trajanje*100));

        dijagnostika.proces_zavrsio(id_procesa);

        cpu_zauzet = false;
        rasporedjuje = true;
        cv_rasp.notify_one();
    }

    // Metoda koju poziva nit koja simulira raspoređivač operativnog sistema, kako bi se izabrao sledeći proces za izvršenje.
    //
    // Ukoliko još uvek ne postoji potreba da se raspoređivač aktivira, potrebno je pozvati metodu dijagnostika.rasporedjivac_ceka a nakon toga nit treba da pređe u stanje čekanja.
    // Nakon što se raspoređivač aktivirao i izabrao sledeći proces, potrebno je pozvati dijagnostika.rasporedjivac_izvrsio.
    void rasporedjuj() {
        unique_lock<mutex> lock(m);

        while(procesi.empty()){
            dijagnostika.rasporedjivac_ceka();
            cv_rasp.wait(lock);
        }

        rasporedjuje = false;

        st_proces *tmp = new st_proces(procesi.front()->id_procesa, procesi.front()->trajanje);
        auto tmp_1 = procesi.begin();
        int min_vreme = procesi.front()->trajanje;
        for(auto it = procesi.begin(); it != procesi.end(); it++){
            if((*it)->trajanje < min_vreme){
                min_vreme = (*it)->trajanje;
                tmp_1 = it;
            }
        }
        procesi.erase(tmp_1);
        dijagnostika.rasporedjivac_izvrsio(tmp->id_procesa);
        tmp->cv.notify_one();
        // NNE RADIIIIII AAAAAAAAAAAAA

    }
};

#endif // RACUNAR_H_INCLUDED
