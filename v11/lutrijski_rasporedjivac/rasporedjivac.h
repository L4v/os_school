#ifndef RASPOREDJIVAC_H_INCLUDED
#define RASPOREDJIVAC_H_INCLUDED

#include <vector>
#include <mutex>
#include <condition_variable>
#include <map>

#include "dijagnostika.h"
#include "cv_tag.h"

using namespace std;

// Struktura koja predstavlja proces
struct Proces {
	int id; // Id procesa
	int broj_naredbi; // Koliko naredbi treba da izvrsi
	vector<int> tiketi; // Tiketi koji pripadaju tom procesu
	Proces(int i, int b, vector<int> t) : id(i), broj_naredbi(b), tiketi(t) {}
};

class Rasporedjivac {
private:
    Dijagnostika& dijagnostika;
    mutex m;
    cv_tag red; // Uslovna promenljiva koja omogucuje da se obavesti preko priveska
    int aktivan_proces; // Proces koji je trenutno zauzeo CPU
    map<int, int> procesi; // Mapa koja spaja tiket i id procesa

public:
    Rasporedjivac(Dijagnostika& d) : dijagnostika(d) {
        aktivan_proces = -1; // CPU je slobodan
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
	        // Dodaje sve tikete procesa u mapu tiketa id-a
	        for(auto it = p.tiketi.begin(); it != p.tiketi.end(); it++)
                procesi[*it] = p.id; // Uvezuje par (tiket, proces)
	    }

	    for(int i = 0; i < p.broj_naredbi; i++){
            unique_lock<mutex> lock(m);

            // U slucaju da je CPU slobodan, zauzima ga proces
            aktivan_proces = aktivan_proces == -1 ? p.id : aktivan_proces;

            // U slucaju da je CPU zauzet, proces ceka
            while(aktivan_proces != p.id){
                dijagnostika.proces_ceka(p.id);
                red.wait(lock, p.id);
            }

            lock.unlock();
            this_thread::sleep_for(chrono::milliseconds(300)); // Izvrsava se proces
            lock.lock();

            // U slucaju da je poslednja naredba, treba izbaciti tikete
            if(i == p.broj_naredbi - 1){
                for(auto it = p.tiketi.begin(); it != p.tiketi.end(); it++){
                    auto tmp = procesi.find(*it); // Trazi tiket vezan za taj proces
                    procesi.erase(tmp); // Brise ga
                }
            }

            // Biranje sledeceg procesa slucajnim izborom
            if(!procesi.empty()){
                int rand_tiket = rand() % procesi.size();
                // Iterator koji ce prolaziti kroz sve tikete
                map<int, int>::iterator it = procesi.begin();
                // Pomeramo iterator na nasumicno odabran tiket
                for(int j = 0; j < rand_tiket; j++, it++);

                aktivan_proces = it->second; // Uzima id procesa iz mape (tiket, id)

                red.notify(aktivan_proces); // Notifikuje adekvatan proces
            }else
                aktivan_proces = -1; // U slucaju da nema vise tiketa, CPU je slobodan

                dijagnostika.izvrsio_naredbu(p.id, i, aktivan_proces);
	    }

	}
};

#endif // RASPOREDJIVAC_H_INCLUDED
