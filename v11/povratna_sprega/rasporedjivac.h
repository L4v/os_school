#ifndef RASPOREDJIVAC_H_INCLUDED
#define RASPOREDJIVAC_H_INCLUDED

#include <condition_variable>
#include <mutex>
#include <vector>

#include "dijagnostika.h"
#include "red.h"

using namespace std;

class Rasporedjivac {
private:
    Dijagnostika& dijagnostika;
    int aktivan_proces;
    mutex m;
    vector<Red> redovi;

public:
	Rasporedjivac(Dijagnostika& d, int broj_nivoa_prioriteta) : dijagnostika(d) {
	    for(int i = 0; i < broj_nivoa_prioriteta; i++)
            redovi.push_back(Red(i));
        aktivan_proces = -1;
	}

	Dijagnostika& getDijagnostika() {
        return dijagnostika;
	}

    // Metoda koju poziva nit koja simulira izvršenje procesa, kako bi se proces izvršio
    //
    // id_procesa   - ID procesa
    // broj_naredbi - ukupan broj naredbi koje proces treba da izvrši
    //
    // Ukoliko je procesor već zauzet i ne mogu se izvršavati naredbe procesa, potrebno je pozvati metodu dijagnostika.proces_ceka a nakon toga proces treba da pređe u stanje čekanja.
    // Nakon što je proces izvršio naredbu, potrebno je pozvati dijagnostika.izvrsio_naredbu.
	void izvrsi(int id_procesa, int broj_naredbi) {
	    int prioritet = 0;
	    for(int i = 0; i < broj_naredbi; i++){
            unique_lock<mutex> lock(m);

            aktivan_proces = aktivan_proces == -1 ? id_procesa : aktivan_proces;
            while(aktivan_proces != id_procesa){
                dijagnostika.proces_ceka(id_procesa);
                redovi[prioritet].dodaj_u_red(id_procesa, lock);
            }

            lock.unlock();
            this_thread::sleep_for(chrono::milliseconds(300));
            lock.lock();

            int obavesti_red = -1;
            for(auto it = redovi.begin(); it != redovi.end(); it++)
                if(!it->prazan()){
                    obavesti_red = it->preuzmi_prioritet();
                    break;
                }

            aktivan_proces = obavesti_red != -1 ? redovi[obavesti_red].izbaci_iz_reda() : -1;

            if(prioritet < redovi.size()-1)
                prioritet++;

            dijagnostika.izvrsio_naredbu(id_procesa, i, prioritet);
	    }
	}
};

#endif // RASPOREDJIVAC_H_INCLUDED
