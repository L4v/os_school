#ifndef TRZNI_CENTAR_H_INCLUDED
#define TRZNI_CENTAR_H_INCLUDED

#include <mutex>
#include <condition_variable>
#include <list>

#include "kupac.h"

using namespace std;

class Trzni_centar {
private:
    mutex m;
    condition_variable free1, free2;
    int kasa1, kasa2;
    int zauzeto1, zauzeto2;
    Kupac& kupac;
public:
    Trzni_centar(Kupac& k) : kupac(k), zauzeto1(0), zauzeto2(0), kasa1(0), kasa2(0){
        // Prosiriti po potrebi ...
    }

    // Metoda koju poziva nit koja simulira kupca kako bi se obavila kupovina.
    // Metoda je blokirajuca - ako su sve kase zauzete, onda se izvrsenje blokira dok se neka ne oslobodi.
    //
    // rbr           - Redni broj kupca
    // broj_artikala - Broj artikala koje kupac zeli da kupi
    //
    // Potrebno je pozvati metodu kupac.ceka kada je izabrana kasa zauzeta i kupac mora da ceka.
    // Potrebno je pozvati metodu kupac.kupuje kada se kasa oslobodi i kupac pocinje kupovinu.
    // Potrebno je pozvati metodu kupac.zavrsio kada je kupac zavrsio kupovinu.
    int kupi(int rbr, int broj_artikala) {
        unique_lock<mutex> lock(m);
        bool prva;
        if(kasa1 <= kasa2){
            prva = true;//aaaaaaaaaaaaaaaaaaaaaaaaaaaa
            kasa1++;
        }else{
            prva = false;
            kasa2++;
        }
        while(kasa1 && kasa2){
            if(kasa1 <= kasa2){
                kupac.ceka(rbr, 1);
                kasa2++;
                free1.wait(lock);
                kasa1--;
            }else{
                kupac.ceka(rbr, 2);
                kasa2++;
                free2.wait(lock);
                kasa2--;
                prva = false;
            }
        }
        kupac.kupuje(rbr, prva ? 1 : 2, broj_artikala);
        lock.unlock();
        this_thread::sleep_for(chrono::seconds(broj_artikala));
        lock.lock();
        kupac.zavrsio(rbr);
        if(prva){
            free1.notify_one();
            return 1;
        }else{
            free2.notify_one();
            return 2;
        }
    }
};


#endif // TRZNI_CENTAR_H_INCLUDED
