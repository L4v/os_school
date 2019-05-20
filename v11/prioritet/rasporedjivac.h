#ifndef RASPOREDJIVAC_H_INCLUDED
#define RASPOREDJIVAC_H_INCLUDED

#include "dijagnostika.h"
#include "cv_hrono.h"
#include <mutex>
#include <queue>
#include <vector>

using namespace std;
using namespace chrono;

class Rasporedjivac {
private:
    Dijagnostika& dijagnostika;
    cv_hrono* cv;
    mutex m;
    int active_proc;
    int num_of_procs;

    vector<deque<int>> procesi;

public:
    Rasporedjivac(Dijagnostika& d, int broj_nivoa_prioriteta) : dijagnostika(d) {
        cv = new cv_hrono[broj_nivoa_prioriteta];
        active_proc = -1;
        num_of_procs = broj_nivoa_prioriteta;
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

            while(active_proc != -1){
                dijagnostika.proces_ceka(id_procesa);
                procesi[prioritet].push_back(id_procesa);
                cv[prioritet].wait(lock);
            }

            active_proc = id_procesa;
            dijagnostika.proces_kreiran(id_procesa, prioritet, broj_naredbi);

            // Izvrsavanje
            lock.unlock();
            this_thread::sleep_for(milliseconds(300));
            lock.lock();


        }
	}
};

#endif // RASPOREDJIVAC_H_INCLUDED
