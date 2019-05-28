#ifndef RACUNAR_H_INCLUDED
#define RACUNAR_H_INCLUDED

#include <condition_variable>
#include <mutex>
#include <vector>
#include <algorithm>

#include "dijagnostika.h"

using namespace std;

class Racunar {
private:
    // Struktura koja predstavlja pojedinacni proces
    struct st_proces{
        int id_procesa, trajanje; // Id procesa i koliko se dugo izvrsava
        condition_variable cv; // Uslovna promenljiva za svaki proces
        st_proces(int i, int j):id_procesa(i), trajanje(j){}
    };

    Dijagnostika& dijagnostika;
    mutex m;
    condition_variable cv_rasp; // Uslovna promenljiva rasporedjivaca
    vector<st_proces*> procesi; // Vektor svih procesa, pokazivac zbog cv
    bool rasporedjuje; // Da li se trenutno vrsi raspodela
    bool zavrsi; // Zbog bug-a u compiler-u
    int aktivan_proces; // Oznacava koji je proces trenutno aktivan

public:
    Racunar(Dijagnostika& d) : dijagnostika(d) {
        aktivan_proces = -1;
        rasporedjuje = false;
        zavrsi = false;
    }

    Dijagnostika& getDijagnostika() {
        return dijagnostika;
    }
    // Gasi beskonacnu petlju rasporedjivaca
    void ugasi(){
        zavrsi = true;
        cv_rasp.notify_one();
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

        // Proces se odmah dodaje u vektor procesa
        st_proces* p = new st_proces(id_procesa, trajanje);
        procesi.push_back(p);

        // U slucaju da nema aktivnog procesa (-1), onda sledeci proces
        // postaje aktivan
        aktivan_proces = aktivan_proces == -1 ? id_procesa : aktivan_proces;

        // U slucaju da je procesor zauzet, trenutni proces ceka
        while(aktivan_proces != id_procesa){
            dijagnostika.proces_ceka(id_procesa);
            p->cv.wait(lock);
        }

        //aktivan_proces = id_procesa;

        lock.unlock();
        this_thread::sleep_for(chrono::milliseconds(trajanje*100)); // Proces radi
        lock.lock();

        // Izbacuje izvrseni proces iz vektora procesa
        for(auto it = procesi.begin(); it != procesi.end(); it++)
            if(*it == p){
                procesi.erase(it);
                delete p;
                break;
            }

        dijagnostika.proces_zavrsio(id_procesa);

        // Poziva rasporedjivac
        rasporedjuje = true;
        cv_rasp.notify_one();
    }

    // Metoda koju poziva nit koja simulira raspoređivač operativnog sistema, kako bi se izabrao sledeći proces za izvršenje.
    //
    // Ukoliko još uvek ne postoji potreba da se raspoređivač aktivira, potrebno je pozvati metodu dijagnostika.rasporedjivac_ceka a nakon toga nit treba da pređe u stanje čekanja.
    // Nakon što se raspoređivač aktivirao i izabrao sledeći proces, potrebno je pozvati dijagnostika.rasporedjivac_izvrsio.
    void rasporedjuj() {
        unique_lock<mutex> lock(m);
        while(!zavrsi){ // Glavna petlja

            // Ceka dok ne dobije signal
            while(!rasporedjuje){
                dijagnostika.rasporedjivac_ceka();
                cv_rasp.wait(lock);
            }
            // U slucaju da je dobio signal da zavrsi, gasi se
            if(zavrsi)return;

            // Dokle god postoje procesi u vektoru, trazi onaj koji ima
            // najkrace vreme izvrsavanja
            if(procesi.size() > 0){
                // Sluzi za cuvanje procesa sa najkracim vremenom
                auto tmp = procesi.begin();
                for(auto it = procesi.begin(); it != procesi.end(); it++)
                    if((*it)->trajanje < (*tmp)->trajanje)
                        tmp = it;

                // Aktivan proces postaje onaj koji ima najkrace vreme
                aktivan_proces = (*tmp)->id_procesa;
                (*tmp)->cv.notify_one();
            }else{
                // U slucaju da nije bilo procesa, CPU je slobodan
                aktivan_proces = -1;
            }

            // Zavrseno rasporedjivanje
            rasporedjuje = false;
            dijagnostika.rasporedjivac_izvrsio(aktivan_proces);

        }
    }
};

#endif // RACUNAR_H_INCLUDED
