#ifndef MEMORIJA_H_INCLUDED
#define MEMORIJA_H_INCLUDED

#include <mutex>
#include <condition_variable>
#include <vector>

#include "dijagnostika.h"

using namespace std;
using namespace chrono;

enum Stanje {SLOBODAN, CITANJE, UPIS };

class Memorija {
private:

    // Predstavlja blok memorije
    struct Blok{
        Stanje stanje; // Stanje bloka
        int ceka, cita; // cita - broj niti koji citaju, ceka - broj niti koje cekaju
        condition_variable cv_upis, cv_citanje;
        Blok() : ceka(0), cita(0), stanje(SLOBODAN){}

    };

    Dijagnostika& dijagnostika;
    mutex m;
    vector<char> lokacije;
    vector<Blok*> blokovi; // Stanja reci
    int br_lokacija;

public:
    Memorija(Dijagnostika& d, int bajtova) : dijagnostika(d) {
        br_lokacija = bajtova;
        blokovi.resize((bajtova + 3)/4); // (5 + 3)/4 = 2
        lokacije.resize(br_lokacija);
        for(int i = 0; i < bajtova; i ++)
            lokacije[i] = -1;

        for(int i = 0; i < blokovi.size(); i ++)
            blokovi.push_back(new Blok);
    }

    ~Memorija(){
        for(auto it = blokovi.begin(); it != blokovi.end(); it++)
            delete *it;
    }

    // Metoda koju poziva nit koja simulira proces koji pristupa memoriji kako bi obavila čitanje iz nje
    //
    // rbp    - Redni broj procesa
    // adresa - Lokacija sa koje se čita vrednost prethodno upisana u memoriju
    //
    // Potrebno je pozvati dijagnostika.proces_ceka_citanje kada je memorijska reč zaključana i proces mora da čeka.
    // Potrebno je pozvati dijagnostika.proces_procitao onda kada je ostvaren pristup memoriji i kada je vrednost učitana iz nje.
    char citaj(int rbp, int adresa) {
        unique_lock<mutex> lock(m);
        int trenutni_blok = adresa / 4;
        while(blokovi[trenutni_blok]->stanje == UPIS){
            dijagnostika.proces_ceka_citanje(rbp, adresa);
            (blokovi[trenutni_blok]->ceka)++;
            blokovi[trenutni_blok]->cv_citanje.wait(lock);
            (blokovi[trenutni_blok]->ceka)--;
        }

        blokovi[trenutni_blok]->stanje = UPIS;
        (blokovi[trenutni_blok]->cita)++;

        lock.unlock();
        this_thread::sleep_for(seconds(1));
        lock.lock();



        if(--(blokovi[trenutni_blok]->cita) == 0){
            blokovi[trenutni_blok]->stanje = SLOBODAN;
            blokovi[trenutni_blok]->cv_upis.notify_one();

        }

        dijagnostika.proces_procitao(rbp, adresa, lokacije[adresa]);

        return lokacije[adresa];
    }

    // Metoda koju poziva nit koja simulira proces koji pristupa memoriji kako bi obavila upis u nju
    //
    // rbp      - Redni broj procesa
    // adresa   - Lokacija u koju se upisuje nova vrednost
    // vrednost - Vrednost koja se upisuje u memoriju
    //
    // Potrebno je pozvati dijagnostika.proces_ceka_upis kada je memorijska reč zaključana i proces mora da čeka.
    // Potrebno je pozvati dijagnostika.proces_upisao onda kada je ostvaren pristup memoriji i kada je vrednost upisana u nju.
    void pisi(int rbp, int adresa, char vrednost) {
        unique_lock<mutex> lock(m);
        int trenutni_blok = adresa / 4;
        while(blokovi[trenutni_blok]->stanje != SLOBODAN){
            dijagnostika.proces_ceka_upis(rbp, adresa);
            blokovi[trenutni_blok]->cv_upis.wait(lock);
        }

        blokovi[trenutni_blok]->stanje = UPIS;

        lock.unlock();
        this_thread::sleep_for(seconds(1));
        lock.lock();
        lokacije[adresa] = vrednost;

        dijagnostika.proces_upisao(rbp, adresa, vrednost);

        if(blokovi[trenutni_blok]->ceka)
            blokovi[trenutni_blok]->cv_citanje.notify_one();
        else
            blokovi[trenutni_blok]->cv_upis.notify_one();

    }
};

#endif // MEMORIJA_H_INCLUDED
