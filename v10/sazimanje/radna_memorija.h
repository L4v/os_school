#ifndef RADNA_MEMORIJA_H_INCLUDED
#define RADNA_MEMORIJA_H_INCLUDED

#include <vector>
#include <condition_variable>
#include <mutex>

#include "dijagnostika.h"

using namespace std;
using namespace chrono;

class Radna_memorija {
private:
    Dijagnostika& dijagnostika;
    mutex m;
    condition_variable cv_proces, cv_sazimanje;
    bool sazima;
    int slobodan_prostor;
    vector<int> memorija;


    /*
    * Trazi pocetak prvog slobodnog bloka odredjene velicine
    */
    int pocetak_bloka(int velicina){
        int pocetak = 0;
        int uzastopni = 0;
        for(auto it = memorija.begin(); it != memorija.end(); it++, pocetak++){
            uzastopni = *it == -1 ? ++uzastopni : 0;
            if(uzastopni == velicina){
                return pocetak - velicina + 1;
            }
        }
        return -1;
    }

public:
    Radna_memorija(Dijagnostika& d, int ukupno_lokacija) : dijagnostika(d) {
        slobodan_prostor = ukupno_lokacija;
        memorija.resize(ukupno_lokacija);
        for(auto it = memorija.begin(); it != memorija.end(); it++)
            *it = -1;
        sazima = false;
    }

    Dijagnostika& getDijagnostika() {
        return dijagnostika;
    }

    // Metoda koju poziva nit koja simulira izvršenje procesa kako bi proces zauzeo potrebnu memoriju i krenuo sa radom
    //
    // id_procesa          - identifikator procesa
    // br_lokacija_procesa - ukupan broj lokacija memorije koje su neophodne za izvršenje procesa
    // trajanje            - koliko dugo se proces izvršava (u sekundama)
    //
    // Ukoliko trenutno nema na raspolaganju dovoljno slobodne memorije (moraju se zauzeti uzastopni okviri u memoriji!), potrebno je pozvati metodu dijagnostika.proces_ceka a nakon toga proces treba da čeka dok se memorija ne oslobodi.
    // Kada proces uspe da dođe do potrebne radne memorije, treba da se pozove dijagnostika.proces_zauzeo_okvire.
    // Kada se proces završi, potrebno je pozvati metodu dijagnostika.proces_zavrsio.
    // Metodu dijagnostika.ispisi_memoriju potrebno je pozvati u sledećim momentima: nakon što proces zauzme memoriju i nakon što proces oslobodi memoriju.
    void koristi(int id_procesa, int br_lokacija_procesa, int trajanje) {
        unique_lock<mutex> lock(m);
        int pocetak;
        // Ulazi u cekanje ukoliko ne postoji dovoljno velik blok ili je sazimanje u toku
        while((pocetak = pocetak_bloka(br_lokacija_procesa)) == -1 || sazima){
            dijagnostika.proces_ceka(id_procesa);
            cv_proces.wait(lock);
        }
        dijagnostika.proces_krenuo(id_procesa, br_lokacija_procesa);

        // Zauzima memoriju
        for(int i = 0; i < br_lokacija_procesa; i++)
            memorija[i+pocetak] = id_procesa;

        dijagnostika.ispisi_memoriju(memorija.begin(), memorija.end());

        lock.unlock();
        this_thread::sleep_for(chrono::seconds(trajanje));
        lock.lock();

        // U slucaju da je doslo do sazimanja u toku izvrsavanja procesa
        for(pocetak = 0; pocetak < memorija.size(); pocetak++)
            if(memorija[pocetak] == id_procesa)break;

        // Oslobadja memoriju
        for(int i = 0; i < br_lokacija_procesa; i++)
            memorija[i+pocetak] = -1;

        dijagnostika.proces_zavrsio(id_procesa);
        dijagnostika.ispisi_memoriju(memorija.begin(), memorija.end());

        sazima = true;
        cv_sazimanje.notify_one();
    }

    // Metoda koju poziva nit operativnog sistema koji se bavi sažimanjem memorije
    //
    // Nakon što je sažimanje obavljeno potrebno je pozvati metodu dijagnostika.sazimanje_obavljeno a nakon toga i metodu dijagnostika.ispisi_memoriju kako bi se prikazali efekti sažimanja memorije.
    void sazimanje() {
        unique_lock<mutex> lock(m);
        while(!sazima)
            cv_sazimanje.wait(lock);
        sazima = false;

        /*vector<int>::iterator it_free = memorija.begin(); // iterator za slobodne bajtove
        for(auto it = memorija.begin(); it != memorija.end(); it++){
            it_free = *it != -1 ? it_free++ : it_free; // Ide do prvog slobodnog bajta
            while(*it != -1){
                *it_free++ = *it;
                *it++ = -1;
            }
        }*/

        vector<int> novi_vektor;
        novi_vektor.resize(memorija.size(), -1);
        vector<int>::iterator it_novi = novi_vektor.begin();
        for(auto it = memorija.begin(); it != memorija.end(); it++)
            if(*it != -1)
                *it_novi++ = *it;

        memorija = novi_vektor;


        dijagnostika.sazimanje_obavljeno();
        dijagnostika.ispisi_memoriju(memorija.begin(), memorija.end());
        cv_proces.notify_all();

    }
};

#endif // RADNA_MEMORIJA_H_INCLUDED
