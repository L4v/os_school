#ifndef RASPOREDJIVAC_H_INCLUDED
#define RASPOREDJIVAC_H_INCLUDED

#include <vector>
#include <map>
#include <mutex>
#include <condition_variable>

#include "dijagnostika.h"
#include "cv_tag.h"

using namespace std;

struct Proces {
	int id;
	int broj_naredbi;
	vector<int> tiketi;
	Proces(int i, int b, vector<int> t) : id(i), broj_naredbi(b), tiketi(t) {}
};

class Rasporedjivac {
private:
    Dijagnostika& dijagnostika;
    int active_process;
    cv_tag red;
    mutex m;
    map<int, int> svi_tiketi;

public:
    Rasporedjivac(Dijagnostika& d) : dijagnostika(d) {
        active_process = -1;
    }

    Dijagnostika& getDijagnostika() {
        return dijagnostika;
    }

    // Metoda koju poziva nit koja simulira izvršenje procesa, kako bi se proces izvršio
    //
    // p - struktura koja sadrži ID procesa, broj naredbi koje treba da izvrši kao i sve tikete koji pripadaju tom procesu.
    //
    // Ukoliko je procesor već zauzet i ne mogu se izvršavati naredbe procesa, potrebno je pozvati metodu dijagnostika.proces_ceka a nakon toga proces treba da pređe u stanje čekanja.
    // Nakon što je proces izvršio naredbu, potrebno je pozvati dijagnostika.izvrsio_naredbu.
	void izvrsi(Proces p) {

	    {
	    unique_lock<mutex> lock(m);
	    for(auto it = p.tiketi.begin(); it != p.tiketi.end(); it++)
            svi_tiketi[*it] = p.id;
	    }

        for(int i = 0; i < p.broj_naredbi; i ++){
            unique_lock<mutex> lock(m);
            if(active_process == -1)
            active_process = p.id;

            while(active_process != p.id){
                dijagnostika.proces_ceka(p.id);
                red.wait(lock, p.id);
            }

            lock.unlock();
            this_thread::sleep_for(milliseconds(300));
            lock.lock();

            if(i == p.broj_naredbi -1)
                for(auto it = p.tiketi.begin(); it != p.tiketi.end(); it++){
                    auto za_brisanje = svi_tiketi.find(*it);
                    svi_tiketi.erase(za_brisanje);
                }
            if(!svi_tiketi.empty()){
                int rand_pos = rand()%svi_tiketi.size();

                map<int, int>::iterator it = svi_tiketi.begin();
                for(int j = 0; j < rand_pos; j++, it++);

                active_process = it->second; // id procesa

                red.notify(active_process);
            }else
                active_process = -1;

            dijagnostika.izvrsio_naredbu(p.id, i, active_process);
        }

	}
};

#endif // RASPOREDJIVAC_H_INCLUDED
