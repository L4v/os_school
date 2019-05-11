#ifndef BIBLIOTEKA_H_INCLUDED
#define BIBLIOTEKA_H_INCLUDED

#include <mutex>
#include <condition_variable>

#include "primerak.h"

using namespace std;

class Biblioteka {
private:
    Primerak& primerak;
    condition_variable free;
    int books;
    mutex m;
public:
    Biblioteka(Primerak& pr, int br) : primerak(pr), books(br) {
        // Prosiriti po potrebi ...
    }

    // Metoda koju poziva nit koja simulira korisnika biblioteke kada on zeli da iznajmi knjigu.
    // Metoda je blokirajuca - ukoliko nema slobodnih primeraka knjige, stajace u stanju cekanja dok se neki ne oslobodi.
    //
    // rbr - Redni broj clana
    //
    // Potrebno je pozvati metodu primerak.ceka kada nema slobodnih primeraka knjige.
    // Potrebno je pozvati metodu primerak.iznajmljen kada nema slobodnih primeraka knjige.
    void iznajmi(int rbr) {
        unique_lock<mutex> lock(m);
        while(books == 0){
            primerak.ceka(rbr);
            free.wait(lock);
        }
        books--;
        primerak.iznajmljen(rbr);
    }

    // Metoda koju poziva nit koja simulira korisnika biblioteke kada on zeli da vrati knjigu koju je prethodno iznajmio.
    //
    // rbr - Redni broj clana
    //
    // Potrebno je pozvati metodu primerak.vracen kada je primerak vracen u biblioteku.
    void vrati(int rbr) {
        unique_lock<mutex> lock(m);
        books++;
        primerak.vracen(rbr);
        free.notify_one();
    }
};

#endif // BIBLIOTEKA_H_INCLUDED
