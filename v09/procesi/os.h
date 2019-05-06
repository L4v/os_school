#ifndef OS_H_INCLUDED
#define OS_H_INCLUDED

#include <mutex>
#include <condition_variable>
#include <queue>

#include "dijagnostika.h"
#include "proces.h"

using namespace std;

struct IO_request{
    condition_variable cv;
    int id_process;
    bool handled;
    IO_request(int i): id_process(i), handled(false){}
};

class OS {
private:
    Dijagnostika& dijagnostika;
    condition_variable cv_process, cv_io;
    int max_std_instructions;
    int active_process;
    mutex m;
    queue<IO_request*> IO_queue;


public:
    // d                 - Referenca na objekat Dijagnostika, koji služi za ispis stanja simulacije
    // maks_uzastopnih_i - Maksimalan dozvoljen broj uzastopnih STANDARD instrukcija, nakon kojeg obavezno sledi preključivanje
    OS(Dijagnostika& d, int maks_uzastopnih_i) : dijagnostika(d), max_std_instructions(maks_uzastopnih_i) {
        active_process = -1;
    }

    // Pomoćna metoda koja vraća objekat klase Dijagnostika koji služi za ispis stanja simulacije
    Dijagnostika& getDijagnostika() {
        return dijagnostika;
    }

    // Metoda koju poziva nit koja simulira proces kako bi se izvršile sve instrukcije procesa.
    //
    // p - Struktura koja sadrži opis procesa (ID procesa i instrukcije koje treba izvršiti)
    //
    // Ukoliko proces ne može da se izvrši (procesor je zauzet), proces mora da se uveže u red spremnih i treba pozvati metodu dijagnostika.proces_ceka.
    // Ukoliko dođe do toga da izvršenje procesa prestaje (dostignut je maksimalan dozvoljen broj uzastopnih STANDARD instrukcija ili su sve instrukcije procesa izvršene), potrebno je pozvati dijagnostika.proces_oslobadja_procesor.
    // Kada se izvrši STANDARD instrukcija nekog procesa, potrebno je pozvati dijagnostika.proces_izvrsio_standardnu.
    // Kada se izvršava UI instrukcija, potrebno je pozvati par metoda: dijagnostika.proces_ceka_na_UI kada se pokrene U/I operacija (nakon čega sledi čekanje na U/I podsistem) i dijagnostika.proces_zavrsio_UI kada se završi U/I operacija.
	void izvrsi_proces(Proces& p) {
        int std_instructions = 0;
        for(vector<INS_TYPE>::iterator it = p.instrukcije.begin(); it != p.instrukcije.end(); it++){
            unique_lock<mutex> lock(m);
            while(active_process != p.id && active_process != -1){
                dijagnostika.proces_ceka(p.id);
                cv_process.wait(lock);
            }
            active_process = p.id;

            if(*it == STANDARD){
                lock.unlock();
                this_thread::sleep_for(chrono::milliseconds(rand()%40 + 11));
                lock.lock();
                dijagnostika.proces_izvrsio_standardnu(p.id);

                if(++std_instructions == max_std_instructions || it == p.instrukcije.end()-1){
                    dijagnostika.proces_oslobadja_procesor(p.id);
                    std_instructions = 0;
                    active_process = -1;
                    cv_process.notify_one();

                    lock.unlock();
                    this_thread::sleep_for(chrono::milliseconds(100));
                    //lock.lock();
                }

            }else{
                IO_request* z= new IO_request(p.id);
                IO_queue.push(z);
                cv_io.notify_one();

                dijagnostika.proces_ceka_na_UI(p.id);
                while(!z->handled){
                    std_instructions = 0;
                    active_process = -1;
                    cv_process.notify_one();
                    z->cv.wait(lock);
                }
                dijagnostika.proces_zavrsio_UI(p.id);
                delete z;
            }
        }
	}

    // Metoda koju poziva nit koja simulira obrađivač U/I zahteva kako bi se obradili trenutno pohranjeni U/I zahtevi.
    //
    // Potrebno je pozvati dijagnostika.ui_ceka kada ne postoje trenutno pohranjeni U/I zahtevi i obrađivač čeka na prvi.
    // Potrebno je pozvati dijagnostika.ui_zapocinje kada obrađivač krene u obradu U/I zahteva. Kada obrađivač završi taj zahtev, potrebno je pozvati dijagnostika.ui_zavrsio.
	void obradi_zahteve() {
	    while(true){
            unique_lock<mutex> lock(m);
            while(IO_queue.empty()){
                dijagnostika.ui_ceka();
                cv_io.wait(lock);
            }

            IO_request* z = IO_queue.front();
            IO_queue.pop();

            dijagnostika.ui_zapocinje(z->id_process);

            lock.unlock();
            this_thread::sleep_for(chrono::milliseconds(rand()%200 + 101));
            lock.lock();

            z->handled = true;
            z->cv.notify_one();

            dijagnostika.ui_zavrsio(z->id_process);

	    }
	}
};

#endif // OS_H_INCLUDED
