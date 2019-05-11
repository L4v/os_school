#ifndef MEMORIJA_H_INCLUDED
#define MEMORIJA_H_INCLUDED

#include <mutex>
#include <condition_variable>

#include "dijagnostika.h"

using namespace std;
using namespace chrono;

enum Stanje {SLOBODAN, CITANJE, UPIS };

class Memorija {
private:
    Dijagnostika& dijagnostika;
    condition_variable* cv_write, *cv_read;
    mutex m;
    Stanje* states;
    char* byte;
    int read_requests, active_readers;

public:
    Memorija(Dijagnostika& d, int bajtova) : dijagnostika(d), read_requests(0), active_readers(0) {
        this->cv_write = new condition_variable[bajtova%4 + 1];
        this->cv_read = new condition_variable[bajtova%4 + 1];
        this->byte = new char[bajtova];
        for(int i = 0; i < bajtova; i++)
            this->byte[i] = 0;
        states ono::seconds(1));= new Stanje[bajtova%4 + 1];
        for(int i = 0; i < bajtova%4; i++)
            states[i] = SLOBODAN;
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
        while(states[adresa%4] == UPIS){
            read_requests++;
            dijagnostika.proces_ceka_citanje(rbp, adresa);
            cv_read[adresa%4].wait(lock);
            read_requests--;
        }
        states[adresa%4] = CITANJE;
        dijagnostika.proces_ceka_citanje(rbp, adresa);

        lock.unlock();
        this_thread::sleep_for(chrono::seconds(1));
        lock.lock();

        dijagnostika.proces_procitao(rbp, adresa, byte[adresa]);
        states[adresa%4] = SLOBODAN;
        /*if(read_requests)
            cv_read[adresa%4].notify_all();
        else*/
        cv_write[adresa%4].notify_one();
        return byte[adresa];
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
        while(states[adresa%4] != SLOBODAN || read_requests != 0){
            dijagnostika.proces_ceka_upis(rbp, adresa);
            cv_write[adresa%4].wait(lock);
        }
        states[adresa%4] = UPIS;

        lock.unlock();
        this_thread::sleep_for(chrono::seconds(1));
        lock.lock();

        byte[adresa%4] = vrednost;
        dijagnostika.proces_upisao(rbp, adresa, vrednost);
        states[adresa%4] = SLOBODAN;

        if(read_requests)
            cv_read[adresa%4].notify_all();
        else
            cv_write[adresa%4].notify_one();
    }
};

#endif // MEMORIJA_H_INCLUDED
