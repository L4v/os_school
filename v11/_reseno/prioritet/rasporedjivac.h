#ifndef RASPOREDJIVAC_H_INCLUDED
#define RASPOREDJIVAC_H_INCLUDED

#include <mutex>
#include <vector>
#include "dijagnostika.h"
#include "cv_hrono.h"
#include "red.h"

using namespace std;
using namespace chrono;

class Rasporedjivac {
private:
    Dijagnostika& dijagnostika;
    mutex m;
    int active_proc;
    vector<Red> redovi;

public:
    Rasporedjivac(Dijagnostika& d, int broj_nivoa_prioriteta) : dijagnostika(d) {
       active_proc = -1;
       for(int i = 0; i < broj_nivoa_prioriteta; i++){
            redovi.push_back(Red(i));
       }
    }

    Dijagnostika& getDijagnostika() {
        return dijagnostika;
    }

    // Metoda koju poziva nit koja simulira izvršenje procesa, kako bi se proces izvršio
    //
    // id_procesa   - ID procesa
    // broj_naredbi - ukupan broj naredbi koje proces treba da izvrši
    // prioritet    - prioritet procesa
    //
    // Ukoliko je procesor već zauzet i ne mogu se izvršavati naredbe procesa, potrebno je pozvati metodu dijagnostika.proces_ceka a nakon toga proces treba da pređe u stanje čekanja.
    // Nakon što je proces izvršio naredbu, potrebno je pozvati dijagnostika.izvrsio_naredbu.
	void izvrsi(int id_procesa, int broj_naredbi, int prioritet) {
        for(int i = 0; i < broj_naredbi; i ++){
            unique_lock<mutex> lock(m);

            if(active_proc == -1)
                active_proc = id_procesa;

            while(active_proc != id_procesa){
                dijagnostika.proces_ceka(id_procesa);
                redovi[prioritet].dodaj_u_red(id_procesa, lock);
            }

            // Izvrsavanje
            lock.unlock();
            this_thread::sleep_for(milliseconds(300));
            lock.lock();

            int obavesti_red = -1;
            for(auto it = redovi.begin(); it != redovi.end(); it++){
                if(!it->prazan()){
                    obavesti_red = it->preuzmi_prioritet();
                    break;
                }
            }

            if(obavesti_red != -1){
                int sledeci;
                if(obavesti_red > prioritet && i < broj_naredbi-1)
                    sledeci = id_procesa;
                else{
                    sledeci = redovi[obavesti_red].izbaci_iz_reda();
                }
                active_proc = sledeci;
            }else
                active_proc = -1;
            dijagnostika.izvrsio_naredbu(id_procesa, i);
        }
	}
};

#endif // RASPOREDJIVAC_H_INCLUDED
