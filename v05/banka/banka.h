#ifndef BANKA_H_INCLUDED
#define BANKA_H_INCLUDED

#include <mutex>
#include <condition_variable>

#include "valuta.h"
#include "kredit.h"

using namespace std;

class Banka {
private:
    Kredit& kredit;
    mutex m;
    condition_variable free_rsd, free_eur;
    int eur, rsd;
public:

    Banka(Kredit& kr, int inicijalni_dsaldo, int inicijalni_esaldo)
        : kredit(kr), eur(inicijalni_esaldo), rsd(inicijalni_dsaldo) {
        // Prosiriti po potrebi ...
    }

    // Metoda koju poziva nit koja simulira klijenta banke, kada klijent zatraži kredit od banke.
    // Ukoliko banka nema odgovarajuća sredstva kako bi omogućila kredit, metoda će blokirati dok se ta sredstva ne oslobode.
    //
    // rbr - Redni broj klijenta
    // svota - Novčani iznos koji je potreban klijentu
    // valuta - Valuta u kojem je novčani iznos
    //
    // Potrebno je pozvati metodu kredit.ceka kada nema sredstava odgovarajuće valute da se odobri kredit.
    // Potrebno je pozvati metodu kredit.dobio kada se kredit realizuje.
    void uzmi_kredit(int rbr, int svota, Valuta valuta) {
        unique_lock<mutex> lock(m);
        if(valuta == 0){
            while((rsd - svota) < 0){
                kredit.ceka(rbr, svota, valuta);
                free_rsd.wait(lock);
            }
            rsd -= svota;
            kredit.dobio(rbr, svota, rsd, valuta);
        }else{
            while((eur - svota) < 0){
                kredit.ceka(rbr, svota, valuta);
                free_eur.wait(lock);
            }
            eur -= svota;
            kredit.dobio(rbr, svota, eur, valuta);
        }
    }

    // Metoda koju poziva nit koja simulira klijenta banke, kada klijent vrati kredit koji je prethodno uzeo od banke.
    //
    // rbr - Redni broj klijenta
    // svota - Novčani iznos koji je potreban klijentu
    // valuta - Valuta u kojem je novčani iznos
    //
    // Potrebno je pozvati metodu kredit.vratio kada je kredit vraćen.
    void vrati_kredit(int rbr, int svota, Valuta valuta) {
       unique_lock<mutex> lock(m);
       if(valuta == 0){
            rsd += svota;
            kredit.vratio(rbr, svota, rsd, valuta);
            free_rsd.notify_all();
       }else{
            eur += svota;
            kredit.vratio(rbr, svota, eur, valuta);
            free_eur.notify_all();
       }
    }
};

#endif // BANKA_H_INCLUDED
