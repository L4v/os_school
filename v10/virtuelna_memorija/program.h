#ifndef PROGRAM_H_INCLUDED
#define PROGRAM_H_INCLUDED

#include <condition_variable>
#include <mutex>

#include "dijagnostika.h"
#include "naredba.h"
#include "povratna_vrednost.h"

using namespace std;

class Program {
private:
    Dijagnostika& dijagnostika;
    mutex m;
    int slobodno; // Koliko slobodnog RAM prostora ima
    int zauzmi_vm; // Koliko treba zauzeti vm
    int zauzeti_vm; // Koliko je zauzeto
    bool zauzimanje_vm; // Da li se vrsi zauzimanje virtuelne memorije
    condition_variable cv_virt; // Signalizira pocetak zauzimanja vm
    condition_variable cv_zavrseno; // Signalizira zavrsetak zauzimanja vm

public:
    Program(Dijagnostika& d, int kapacitet) : dijagnostika(d) {
        zauzimanje_vm = false;
        slobodno = kapacitet;
        zauzeti_vm = 0;
    }

    Dijagnostika& getDijagnostika() {
        return dijagnostika;
    }

    // Metoda koju poziva nit koja simulira izvršenje programa, kako bi zauzela potrebnu memoriju
    //
    // naredba - naredba koja se izvršava (naredba kojom se zauzima memorija)
    // Vraća instancu klase Povratna_vrednost koja sadrži opis zauzeća memorije NAKON izvršenja naredbe.
    Povratna_vrednost izvrsi_naredbu(Naredba naredba) {
        unique_lock<mutex> lock(m);
        int alocirani_vm = 0;
        if(naredba.tip.compare("repeat")){ // compare vraca 0 ako su isti
            // Ovde ulazi ako nije repeat
            if(naredba.kolicina_memorije <= slobodno){
                // Ovde ulazi ako moze sve u RAM
                slobodno -= naredba.kolicina_memorije;
            }else{
                zauzmi_vm = naredba.kolicina_memorije;
                zauzimanje_vm = true;
                cv_virt.notify_one();
                while(zauzimanje_vm)
                    cv_zavrseno.wait(lock);
            }
        }else{
            // Ovde ulazi ako je repeat
            for(int i = 0; i < naredba.ponavljanja; i++){
                if(naredba.kolicina_memorije > slobodno){
                    zauzmi_vm = naredba.kolicina_memorije;
                    zauzimanje_vm = true;
                    cv_virt.notify_one();
                    while(zauzimanje_vm)
                        cv_zavrseno.wait(lock);
                }
            }
        }


        return {slobodno, zauzeti_vm};
    }

    // Metoda koju poziva nit koja simulira deo operativnog sistema koji se bavi zauzimanjem virtuelne memorije kako bi se zauzela određena količina VM
    // Potrebnu količinu VM (kao i bilo koju drugu neophodnu informaciju) preneti u nekom izabranom polju klase.
    int zauzmi_virtuelnu_memoriju() {
        unique_lock<mutex> lock(m);
        while(!zauzimanje_vm)
            cv_virt.wait(lock);


        zauzeti_vm += zauzmi_vm;
        zauzmi_vm = 0;

        this_thread::sleep_for(chrono::seconds(1));
        zauzimanje_vm = false;
        cv_zavrseno.notify_one();

        return zauzeti_vm;
    }
};

#endif // PROGRAM_H_INCLUDED
