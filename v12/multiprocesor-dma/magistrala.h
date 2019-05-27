#ifndef MAGISTRALA_H_INCLUDED
#define MAGISTRALA_H_INCLUDED

#include <mutex>
#include <condition_variable>

#include "dijagnostika.h"

using namespace std;

// Klasa magistrale (sabirnice). Glavna deljena promenljiva za sinhronizaciju pristupa procesora memoriji i dma kontroleru.
class Magistrala {
public:
    // Stanja sabirnice - slobodna, čitanje iz memorije, pisanje u memoriju, dma transfer. Primetiti sličnost sa stanjima iz zadatka "multiprocesor":
    enum Stanje { SLOBODNA, MEM_CITAJ, MEM_PISI, DMA};

    // Struktura u koju se beleže parametri DMA transfera. Odakle, kome i koliko bajtova se čita ili piše.
    struct DMA_transfer {
        int odakle;
        int koliko;
        int kome;
    };
private:
    DMA_transfer za_preneti;
    Dijagnostika& dijagnostika;
    Memorija& memorija;
    mutex m;
    condition_variable cv_mag, cv_dma;
    Stanje stanje;
public:

    Magistrala(Dijagnostika& d, Memorija& mem) : dijagnostika(d), memorija(mem) {
        stanje = SLOBODNA;
    }

    Dijagnostika& getDijagnostika() {
        return dijagnostika;
    }

    char citaj_memoriju(int adresa) {
        unique_lock<mutex> lock(m);
        while(stanje != SLOBODNA)
            cv_mag.wait(lock);

        this_thread::sleep_for(chrono::milliseconds(700));
        stanje = MEM_CITAJ;
        lock.unlock();

        char vrednost = memorija.citaj(adresa);
        lock.lock();

        stanje = SLOBODNA;
        cv_mag.notify_one();
        return vrednost;
    }

    void pisi_u_memoriju(int adresa, char vrednost) {
        unique_lock<mutex> lock(m);
        while(stanje != SLOBODNA)
            cv_mag.wait(lock);

        this_thread::sleep_for(chrono::milliseconds(700));

        lock.unlock();
        memorija.pisi(adresa, vrednost);
        lock.lock();
        stanje = SLOBODNA;
        cv_mag.notify_one();
    }

    void dma(DMA_transfer transfer) {
        unique_lock<mutex> lock(m);
        while(stanje != SLOBODNA)
            cv_mag.wait(lock);

        this_thread::sleep_for(chrono::milliseconds(700));
        za_preneti = transfer;
        stanje = DMA;
        cv_dma.notify_one();
    }

    DMA_transfer okidac_dma_kontrolera() {
        unique_lock<mutex> lock(m);
        while(stanje != DMA)
            cv_dma.wait(lock);

        lock.unlock();

        for(int i = 0; i < za_preneti.koliko; i++)
            memorija.pisi(za_preneti.kome+i, memorija.citaj(za_preneti.odakle+i));

        lock.lock();

        stanje = SLOBODNA;
        cv_mag.notify_one();

        return za_preneti;
    }
};

#endif // MAGISTRALA_H_INCLUDED
