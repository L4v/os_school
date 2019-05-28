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
    Memorija& memorija; // Objekat memorije
    mutex m;
    condition_variable cv_mag, cv_dma; // Uslovne promenljive za magistralu i DMA prenos
    Stanje stanje; // Trenutno stanje magistrale
public:

    Magistrala(Dijagnostika& d, Memorija& mem) : dijagnostika(d), memorija(mem) {
        stanje = SLOBODNA;
    }

    Dijagnostika& getDijagnostika() {
        return dijagnostika;
    }
    /*
    * Cita iz memorije, sa odredjene adrese
    */
    char citaj_memoriju(int adresa) {
        unique_lock<mutex> lock(m);
        // U slucaju da je magistrala zauzeta, ceka
        while(stanje != SLOBODNA)
            cv_mag.wait(lock);


        // Simulira zakljucavanje magistrale
        this_thread::sleep_for(chrono::milliseconds(700));

        // Prebacuje magistralu u stanje citanja
        stanje = MEM_CITAJ;

        // Cita memoriju
        lock.unlock();
        char vrednost = memorija.citaj(adresa);
        lock.lock();

        // Magistrala je zavrsila sa citanjem i prelazi u stanje SLOBODNA
        stanje = SLOBODNA;

        // Obavestava se da je slobodna magistrala
        cv_mag.notify_one();
        return vrednost;
    }

    /*
    * Upisuje vrednost na odredjenu memorijsku lokaciju
    */
    void pisi_u_memoriju(int adresa, char vrednost) {
        unique_lock<mutex> lock(m);

        // U slucaju da je magistrala zauzeta, ceka
        while(stanje != SLOBODNA)
            cv_mag.wait(lock);

        // Zakljucavanje magistrale
        this_thread::sleep_for(chrono::milliseconds(700));

        // Vrsi pisanje
        lock.unlock();
        memorija.pisi(adresa, vrednost);
        lock.lock();

        // Oslobadja magistralu
        stanje = SLOBODNA;

        // Obavestava o zavrsetku
        cv_mag.notify_one();
    }

    /*
    * Vrsi DMA transfer (kopira vrednost iz jedne lokacije u drugu)
    */
    void dma(DMA_transfer transfer) {
        unique_lock<mutex> lock(m);

        // U slucaju da je magistrala zauzeta, ceka
        while(stanje != SLOBODNA)
            cv_mag.wait(lock);


        // Zakljucabva magistralu
        this_thread::sleep_for(chrono::milliseconds(700));

        // Postavlja zeljenu vrednost transfera
        za_preneti = transfer;

        // Magistrala prelazi u stanje DMA
        stanje = DMA;

        // Obavestava se DMA okidac
        cv_dma.notify_one();
    }

    DMA_transfer okidac_dma_kontrolera() {
        unique_lock<mutex> lock(m);

        // Ceka dok ne dobije signal
        while(stanje != DMA)
            cv_dma.wait(lock);

        // Vrsi se prebacivanje s jedne memorijske lokacije na drugu
        lock.unlock();
        for(int i = 0; i < za_preneti.koliko; i++)
            memorija.pisi(za_preneti.kome+i, memorija.citaj(za_preneti.odakle+i));
        lock.lock();

        // Oslobadja se magistrala i obavestava se da je slobodna
        stanje = SLOBODNA;
        cv_mag.notify_one();

        // Vraca izvrsen transfer
        return za_preneti;
    }
};

#endif // MAGISTRALA_H_INCLUDED
