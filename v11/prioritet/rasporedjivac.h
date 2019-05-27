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
    vector<Red> prioriteti;
    mutex m;
    int cpu_id;

public:
    Rasporedjivac(Dijagnostika& d, int broj_nivoa_prioriteta) : dijagnostika(d) {
        for(int i = 0; i < broj_nivoa_prioriteta; i++)
            prioriteti.push_back(Red(i));
        cpu_id = -1;
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
	    for(int i = 0; i < broj_naredbi; i++){
            unique_lock<mutex> lock(m);

            cpu_id = cpu_id == -1 ? id_procesa : cpu_id;

            while(cpu_id != id_procesa){
                dijagnostika.proces_ceka(id_procesa);
                prioriteti[prioritet].dodaj_u_red(id_procesa, lock);
            }

            lock.unlock();
            this_thread::sleep_for(milliseconds(300));
            lock.lock();

            int notif_red = -1;
            for(auto it = prioriteti.begin(); it != prioriteti.end(); it++){
                if(!it->prazan()){
                    notif_red = it->getPrioritet();
                    break;
                }
            }

            if(notif_red != -1){
                int sledeci;
                if(notif_red > prioritet && i < broj_naredbi - 1){
                    sledeci = id_procesa; // Ako jos nije zavrsio i najveci je prioritet trenutni proces
                }else{
                    sledeci = prioriteti[notif_red].izbaci();
                }
                cpu_id = sledeci; // Uzima sledeci proces
            }else{
                cpu_id = -1; // Inace nema aktivnog procesa
            }
            dijagnostika.izvrsio_naredbu(id_procesa, i);

	    }
	}
};

#endif // RASPOREDJIVAC_H_INCLUDED
