#ifndef RADNA_MEMORIJA_H_INCLUDED
#define RADNA_MEMORIJA_H_INCLUDED

#include "dijagnostika.h"
#include <vector>
#include <mutex>
#include <condition_variable>

using namespace std;
using namespace chrono;

class Radna_memorija {
private:
    Dijagnostika& dijagnostika;
    vector<int> memory;
    condition_variable cv_process, cv_defrag;
    int free_space_block;
    bool defrag;
    mutex m;

public:
    Radna_memorija(Dijagnostika& d, int ukupno_lokacija) : dijagnostika(d) {
        int free_space = ukupno_lokacija;
        memory.resize(ukupno_lokacija, -1);
        defrag = false;
    }

    Dijagnostika& getDijagnostika() {
        return dijagnostika;
    }

    int block_begin(int num_of_locs){
        int broj_uzastopnih = 0;
        int i = 0;

        for(auto it = memory.begin(); it != memory.end(); it++, i++){
            if(*it == -1)
                broj_uzastopnih++;
            else
                broj_uzastopnih = 0;
            if(broj_uzastopnih == num_of_locs)
                return i - num_of_locs + 1;
        }
        return -1;
    }

    // Metoda koju poziva nit koja simulira izvršenje procesa kako bi proces zauzeo potrebnu memoriju i krenuo sa radom
    //
    // id_procesa          - identifikator procesa
    // br_lokacija_procesa - ukupan broj lokacija memorije koje su neophodne za izvršenje procesa
    // trajanje            - koliko dugo se proces izvršava (u sekundama)
    //
    // Ukoliko trenutno nema na raspolaganju dovoljno slobodne memorije (moraju se zauzeti uzastopni okviri u memoriji!), potrebno je pozvati metodu dijagnostika.proces_ceka a nakon toga proces treba da čeka dok se memorija ne oslobodi.
    // Kada proces uspe da dođe do potrebne radne memorije, treba da se pozove dijagnostika.proces_zauzeo_okvire.
    // Kada se proces završi, potrebno je pozvati metodu dijagnostika.proces_zavrsio.
    // Metodu dijagnostika.ispisi_memoriju potrebno je pozvati u sledećim momentima: nakon što proces zauzme memoriju i nakon što proces oslobodi memoriju.
    void koristi(int id_procesa, int br_lokacija_procesa, int trajanje) {
        unique_lock<mutex> lock(m);
        int loc_begin = block_begin(br_lokacija_procesa);
        while(loc_begin == -1){
            dijagnostika.proces_ceka(id_procesa);
            cv_process.wait(lock);
        }
        dijagnostika.proces_krenuo(id_procesa, br_lokacija_procesa);

        for(int i = 0; i < br_lokacija_procesa; i ++)
            memory[i+loc_begin] = id_procesa;

        dijagnostika.proces_zauzeo_okvire(id_procesa, loc_begin, loc_begin + br_lokacija_procesa);
        dijagnostika.ispisi_memoriju(memory.begin(), memory.end());

        lock.unlock();
        this_thread::sleep_for(chrono::seconds(trajanje));
        lock.lock();

        for(loc_begin = 0; loc_begin < memory.size(); loc_begin++)
            if(memory[loc_begin] == id_procesa)
                break;

        for(int i = 0; i < br_lokacija_procesa; i++)
            memory[i+loc_begin] = -1;

        dijagnostika.proces_zavrsio(id_procesa);
        dijagnostika.ispisi_memoriju(memory.begin(), memory.end());

        defrag = true;
        cv_defrag.notify_one();

    }

    // Metoda koju poziva nit operativnog sistema koji se bavi sažimanjem memorije
    //
    // Nakon što je sažimanje obavljeno potrebno je pozvati metodu dijagnostika.sazimanje_obavljeno a nakon toga i metodu dijagnostika.ispisi_memoriju kako bi se prikazali efekti sažimanja memorije.
    void sazimanje() {
        unique_lock<mutex> lock(m);
        while(!defrag)
            cv_defrag.wait(lock);


        defrag = false;

        vector<int> tmp;
        tmp.resize(memory.size(), -1);
        int tmp_it = 0;
        for(int i = 0; i < memory.size(); i++)
            if(memory[i] != -1)
                tmp[tmp_it++] = memory[i];

        memory = tmp;

        cv_process.notify_all();

    }
};

#endif // RADNA_MEMORIJA_H_INCLUDED
