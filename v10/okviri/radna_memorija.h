#ifndef RADNA_MEMORIJA_H_INCLUDED
#define RADNA_MEMORIJA_H_INCLUDED

#include <mutex>
#include <condition_variable>
#include <vector>

#include "dijagnostika.h"

using namespace std;

class Radna_memorija {
private:
    Dijagnostika& dijagnostika;
    vector<int> okviri;
    int ukupno_okvira;
    int slobodan_prostor;
    mutex m;
    condition_variable cv;

public:
    // dijagnostika  - referenca na instancu klase Dijagnostika
	// ukupno_okvira - ukupan broj okvira u memoriji
    Radna_memorija(Dijagnostika& d, int ukupno_okvira) : dijagnostika(d) {
        ukupno_okvira = ukupno_okvira;
        okviri.resize(ukupno_okvira);
        slobodan_prostor = ukupno_okvira;

        for(int i = 0; i < ukupno_okvira; i++)
            okviri[i] = -1;
    }

    Dijagnostika& getDijagnostika() {
        return dijagnostika;
    }

    // Metoda koju poziva nit koja simulira izvršenje procesa kako bi pokušala da učita proces u radnu memoriju
    //
    // broj_stranica - ukupan broj stranica koje proces sadrži
    // id_procesa    - identifikator procesa
    //
    // Ukoliko trenutno nema na raspolaganju dovoljno memorije (broj stranica u procesu je veći od broja raspoloživih okvira memorije), potrebno je pozvati metodu dijagnostika.proces_ceka a nakon toga proces treba da čeka dok se memorija ne oslobodi.
    // Kada proces uspe da dođe do potrebne radne memorije, treba da se pozove dijagnostika.proces_izvrsava. Nakon toga,
    // kada proces zauzme okvire radne memorije, potrebno je pozvati dijagnostika.ispisi_okvire kako bi se prikazalo trenutno zauzece svih okvira (podrazumeva se da zelimo da prikazemo sliku svih okvira, tako da ce se videti i okviri koje su zauzeli drugi procesi).
    void ucitaj(int broj_stranica, int id_procesa) {
        unique_lock<mutex> lock(m);
        while(slobodan_prostor < broj_stranica){
            dijagnostika.proces_ceka(id_procesa);
            cv.wait(lock);
        }
        dijagnostika.proces_se_izvrsava(id_procesa);
        int i = broj_stranica;
        for(auto it = okviri.begin(); it != okviri.end(); it++){
            if(*it == -1){
                *it = id_procesa;
                slobodan_prostor--;
                if(!(--i)) // U slucaju da je zauzeo sve lokacije, izlazi iz petlje
                    break;
            }
        }
        dijagnostika.ispisi_okvire(okviri.begin(), okviri.end());

        dijagnostika.proces_se_zavrsio(id_procesa);
    }

    // Metoda koju poziva nit koja simulira izvršenje procesa kako bi oslobodila radnu memoriju koju je koristila tokom izvršenja
    //
    // id_procesa - identifikator procesa koji oslobađa memoriju
    void oslobodi(int id_procesa) {
        unique_lock<mutex> lock(m);
        for(auto it = okviri.begin(); it != okviri.end(); it++)
            if(*it == id_procesa){
                *it = -1;
                slobodan_prostor++;
            }


        cv.notify_all();
    }
};

#endif // RADNA_MEMORIJA_H_INCLUDED
