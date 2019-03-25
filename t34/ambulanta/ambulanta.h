#ifndef AMBULANTA_H_INCLUDED
#define AMBULANTA_H_INCLUDED

#include <mutex>
#include <condition_variable>

#include "student.h"

using namespace std;

class Studentska_ambulanta {
private:
    Student& student;
    int m, z;
    mutex mtx;
    condition_variable free_m, free_z;
public:
    Studentska_ambulanta(Student& st, int muskih, int zenskih) :
        student(st), m(muskih), z(zenskih) {
        // Prosiriti po potrebi ...
    }

    // Metoda koju poziva nit koja simulira studenta u ambulanti kada student pokusava da udje u nju.
    // Ukoliko je ambulanta zauzeta, izvrsenje ove metode ce cekati da se ona oslobodi.
    //
    // rbr - Redni broj studenta
    // p   - Pol studenta
    //
    // Potrebno je pozvati metodu student.ceka kada student ne moze da stupi u ambulantu.
    // Potrebno je pozvati metodu student.ulazi kada student udje u ambulantu.
    void udji(int rbr, Pol p) {
        unique_lock<mutex> lock(mtx);
        if(p == 0){
            while(m == 0){
                student.ceka(rbr, p, m);
                free_m.wait(lock);
            }
            m--;
            student.ulazi(rbr, p, m);
        }else{
            while(z == 0){
                student.ceka(rbr, p, z);
                free_z.wait(lock);
            }
            z--;
            student.ulazi(rbr, p, z);
        }
    }

    // Metoda koju poziva nit koja simulira studenta u ambulanti nakon sto je student pregledan i izlazi iz nje.
    //
    // rbr - Redni broj studenta
    // p   - Pol studenta
    void izadji(int rbr, Pol p) {
        if(p == 0){
            m++;
            free_m.notify_all();
        }else{
            z++;
            free_z.notify_all();
        }
    }
};

#endif // AMBULANTA_H_INCLUDED
