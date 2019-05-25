#ifndef OS_H_INCLUDED
#define OS_H_INCLUDED

#include <vector>
#include <queue>
#include <condition_variable>
#include <mutex>

#include "dijagnostika.h"
#include "proces.h"

using namespace std;

// UI_zahtev sluzi kao pomocna struktura radi lakseg rukovanja sa zahtevima
struct UI_zahtev{
    condition_variable cv; // Uslovna promenljiva za pojedinacni zahtev
    int id; // ID procesa koji vrsi zahtev
    bool izvrseno; // Da li je zahtev izvrsen
    UI_zahtev(int i):id(i), izvrseno(false){} // Konstruktor
};
class OS {


private:
    Dijagnostika& dijagnostika;
    condition_variable cv_proces, cv_ui; // Uslovne promenljive za obicne procese i UI procese
    int aktivan_proces; // Proces koji trenutno drzi CPU, ako -1 onda je CPU slobodan
    int max_std_instr; // Maksimalan broj standardnih instrukcija
    mutex m;
    queue<UI_zahtev*> zahtevi; // Red zahteva koji cekaju

    bool ugasiti;

public:

    void ugasi(){
        // Novija verzija gcc-a ne unistava nit nakon zavrsavanja i time potencijalno
        // izaziva hang, pa ovo sluzi da se "ugasi" obradi_zahteve()
        ugasiti = true;
        cv_ui.notify_one();
    }

    // d                 - Referenca na objekat Dijagnostika, koji služi za ispis stanja simulacije
    // maks_uzastopnih_i - Maksimalan dozvoljen broj uzastopnih STANDARD instrukcija, nakon kojeg obavezno sledi preključivanje
    OS(Dijagnostika& d, int maks_uzastopnih_i) : dijagnostika(d), max_std_instr(maks_uzastopnih_i){
        aktivan_proces = -1;
        ugasiti = false;
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
	    int izvrsene_std_i = 0; // Broj izvrsenih STANDARD instrukcija
	    for(auto it = p.instrukcije.begin(); it != p.instrukcije.end(); it++){
            unique_lock<mutex> lock(m);
            while(aktivan_proces != -1 && aktivan_proces != p.id){
                dijagnostika.proces_ceka(p.id);
                cv_proces.wait(lock);
            }

            aktivan_proces = p.id; // Proces je zauzeo CPU

            if(*it == STANDARD){
                // Izvrsavanje STANDARD procesa

                lock.unlock();
                this_thread::sleep_for(chrono::milliseconds(rand()%40 + 11));
                dijagnostika.proces_izvrsio_standardnu(p.id);
                lock.lock();

                // Provera da li je stigao do kraja izvrsavanja
                if(++izvrsene_std_i == max_std_instr || it == p.instrukcije.end()-1){
                    dijagnostika.proces_oslobadja_procesor(p.id);
                    aktivan_proces = -1;
                    cv_proces.notify_one();

                    lock.unlock();
                    this_thread::sleep_for(chrono::milliseconds(100)); // Cooldown period
                    lock.lock(); // Nepotrebno jer se ovde zavrsava block koda, pa ce se pozvati destruktor lock-a
                }
            }else{
                // Obradjivanje UI procesa

                UI_zahtev* zahtev = new UI_zahtev(p.id);
                zahtevi.push(zahtev);

                cv_ui.notify_one();

                dijagnostika.proces_ceka_na_UI(p.id);
                while(!zahtev->izvrseno){
                    izvrsene_std_i = 0;
                    aktivan_proces = -1;
                    cv_proces.notify_one();
                    zahtev->cv.wait(lock);
                }

                dijagnostika.proces_zavrsio_UI(p.id);
                delete zahtev; // Brisanje zahteva

            }

	    }
	}

    // Metoda koju poziva nit koja simulira obrađivač U/I zahteva kako bi se obradili trenutno pohranjeni U/I zahtevi.
    //
    // Potrebno je pozvati dijagnostika.ui_ceka kada ne postoje trenutno pohranjeni U/I zahtevi i obrađivač čeka na prvi.
    // Potrebno je pozvati dijagnostika.ui_zapocinje kada obrađivač krene u obradu U/I zahteva. Kada obrađivač završi taj zahtev, potrebno je pozvati dijagnostika.ui_zavrsio.
	void obradi_zahteve() {
	    unique_lock<mutex> lock(m);
	    while(true){
            while(zahtevi.empty()){
                // Ukoliko nema UI zahteva, ceka
                dijagnostika.ui_ceka();
                cv_ui.wait(lock);
            }

            if(ugasiti) return;

            UI_zahtev* zahtev = zahtevi.front(); // Uzima sa vrha reda
            zahtevi.pop(); // Izbacuje iz reda

            dijagnostika.ui_zapocinje(zahtev->id);

            lock.unlock();
            this_thread::sleep_for(chrono::milliseconds(rand()%200 + 101));
            lock.lock();

            dijagnostika.proces_zavrsio_UI(zahtev->id);

            zahtev->izvrseno = true;
            zahtev->cv.notify_one();

	    }
	}
};

#endif // OS_H_INCLUDED
