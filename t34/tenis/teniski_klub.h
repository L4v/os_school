#ifndef TENISKI_KLUB_H_INCLUDED
#define TENISKI_KLUB_H_INCLUDED

#include <mutex>
#include <condition_variable>
#include <map>

#include "podaci.h"
#include "mec.h"

#define MAX_TERENA 30

using namespace std;

class Teniski_klub {                             //Klasa deljenog resursa. Pravi se maksimalno 30 terena, ali ce u realnosti biti samo 3 terena.
private:
    Mec& mec;
    int broj_terena;
    map<int, condition_variable> free;
    mutex m;
public:
    Teniski_klub(Mec& m, int ukupno_terena) : mec(m), broj_terena(ukupno_terena <= MAX_TERENA ? ukupno_terena : MAX_TERENA){
    }

    // Metoda koju poziva nit koja simulira mec kako bi izvrsila teniski mec.
    // Metoda je blokirajuca - ako je zeljeni teren zauzet, izvrsenje ce blokirati dok se on ne oslobodi.
    //
    // broj_meca - Redni broj meca
    // na_terenu - Indeks terena na kojem mec treba da se odigra
    //
    // Potrebno je pozvati metodu mec.ceka kada je izabrani teren zauzet i mec mora da ceka.
    // Potrebno je pozvati metodu mec.pocinje kada se isprazni izabrani teren i mec moze da pocne.
    // Potrebno je pozvati metodu mec.zavrsen kada se mec zavrsi i mogu se proslediti izmereni vremenski intervali.
    void odigraj_mec(int broj_meca, int na_terenu) {
        unique_lock<mutex> lock(m);
        Podaci ret;
        ret.dosao = system_clock::now();
        while(broj_terena == 0){
            mec.ceka(broj_meca, na_terenu);
            free[na_terenu].wait(lock);
        }
        broj_terena--;
        system_clock::time_point start = system_clock::now();
        ret.pocetak = start;
        mec.pocinje(broj_meca, na_terenu);
        lock.unlock();
        this_thread::sleep_for(chrono::seconds(rand()%5 + 1));
        lock.lock();
        ret.trajanje = system_clock::now() - start;
        mec.zavrsen(broj_meca, ret);
        broj_terena++;
        free[na_terenu].notify_one();
    }
};

#endif // TENISKI_KLUB_H_INCLUDED
