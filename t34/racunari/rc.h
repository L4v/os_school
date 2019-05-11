#ifndef RC_H_INCLUDED
#define RC_H_INCLUDED

#include <mutex>
#include <condition_variable>
#include <map>

#include "student.h"

using namespace std;

#define MAX 50 //maksimalni broj racunara u ucionici

class RC {
private:
    Student& student;
    mutex m;
    condition_variable free;
    int racunari; // 0 -> svi zauzeti
    int br_rac;
    map<int, int> zauzeti; // mapa student:racunar
    map<int, int> slobodni;// mapa racunar:zauzetost
public:
	RC(Student& st, int br) : student(st) {
	    this->racunari = br <= MAX ? br : MAX;
	    for(int i = 0; i < this->racunari; i ++)
            slobodni[i+1] = 1;
        this->br_rac = racunari;
    }

    // Metoda koju poziva nit koja simulira studenta kako bi student zauzeo mesto za racunarom.
    // Ova metoda je blokirajuca - ako nema slobodnih mesta, ona ce cekati dok se neko ne oslobodi.
    //
    // rbr - Redni broj studenta
    //
    // Potrebno je pozvati metodu student.ceka kada su racunari zauzeti i student mora da ceka.
    // Potrebno je pozvati metodu student.zauzeo kada student zauzme racunar.
    int zauzmi(int rbr) {
        unique_lock<mutex> lock(m);
        while(racunari == 0){
            student.ceka(rbr);
            free.wait(lock);
        }
        int uzet;
        for(int i = 1; i <= br_rac; i ++)
            if(slobodni[i] == 1){
                uzet = i;
                break;
            }
        zauzeti[rbr] = uzet;
        slobodni[uzet] = 0;
        student.zauzeo(rbr, zauzeti[rbr]);
        racunari --;
        return zauzeti[rbr];
    }

    // Metoda koju poziva nit koja simulira studenta kako bi oslobodio racunar koji je prethodno zauzeo.
    //
    // rbr         - Redni broj studenta
    // id_racunara - Redni broj racunara kojeg student oslobadja (prethodno je zauzeo taj racunar)
    //
    // Potrebno je pozvati metodu student.oslobodio kada student oslobodi racunar.
    void oslobodi(int rbr, int id_racunara) {
        student.oslobodio(rbr, zauzeti[rbr]);
        racunari ++;
        slobodni[zauzeti[rbr]] = 1;
        free.notify_all();
    }
};

#endif // RC_H_INCLUDED
