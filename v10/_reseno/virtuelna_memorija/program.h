#ifndef PROGRAM_H_INCLUDED
#define PROGRAM_H_INCLUDED

#include "dijagnostika.h"
#include "naredba.h"
#include "povratna_vrednost.h"
#include <mutex>
#include <condition_variable>
#include <vector>


using namespace std;

class Program {
private:
    Dijagnostika& dijagnostika;
    condition_variable cv_ram, cv_vm;
    int free_space, vm_to_alloc, allocated_vm;
    bool alloc;//vm_alloc, ram_alloc;
    mutex m;

public:
    Program(Dijagnostika& d, int kapacitet) : dijagnostika(d) {
        free_space = kapacitet;
        vm_to_alloc = 0;
        allocated_vm = 0;
        alloc = false;
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
        while(alloc)
            cv_ram.wait(lock);

        //ram_alloc = true;

        Povratna_vrednost ret;

        dijagnostika.pokrenuta_naredba(naredba);

        if(!naredba.tip.compare("malloc")){
            if(naredba.kolicina_memorije > free_space){
                vm_to_alloc = naredba.kolicina_memorije;
                alloc = true;
                cv_vm.notify_one();
                while(alloc)
                    cv_ram.wait(lock);
            }else{
                free_space -= naredba.kolicina_memorije;
            }
        }else{
            for(int i = 0; i < naredba.ponavljanja; i++){
                if(naredba.kolicina_memorije > free_space){
                    vm_to_alloc = naredba.kolicina_memorije;
                    alloc = true;
                    cv_vm.notify_one();
                    while(alloc)
                        cv_ram.wait(lock);
                }else
                    free_space -= naredba.kolicina_memorije;
            }
        }

        ret.slobodno_ram = free_space;
        ret.zauzeto_u_virtuelnoj = allocated_vm;
        dijagnostika.izvrsena_naredba(naredba, free_space, allocated_vm);
        //ram_alloc = false;

        return ret;
    }

    // Metoda koju poziva nit koja simulira deo operativnog sistema koji se bavi zauzimanjem virtuelne memorije kako bi se zauzela određena količina VM
    // Potrebnu količinu VM (kao i bilo koju drugu neophodnu informaciju) preneti u nekom izabranom polju klase.
    int zauzmi_virtuelnu_memoriju() {
        unique_lock<mutex> lock(m);

        while(!alloc)
            cv_vm.wait(lock);

        alloc = false;
        allocated_vm += vm_to_alloc;
        cout << allocated_vm << " " << vm_to_alloc << endl;
        dijagnostika.izvrseno_zauzimanje_vm(allocated_vm);
        cv_ram.notify_one();
        return allocated_vm;
    }
};

#endif // PROGRAM_H_INCLUDED
