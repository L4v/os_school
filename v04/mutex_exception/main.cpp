/*
Napraviti konkurentni program u kom se u funkciji niti pravi veliki niz od 1.000.000.000.000 elemenata. Niz se pravi dinamički. Kreiranje niza zaštititi try - catch blokom. U okviru try catch bloka zaključati mutex pre pravljenja niza i otključati ga nakon pravljenja niza.

Posmatrati ponašanje programa.

Nakon toga promeniti kod tako da se ne zaključava mutex eksplicitno, već da se koristi klasa unique_lock.
*/


#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

mutex m;
const unsigned long long NUM = 1000000000000;

void kreiraj(){
    int * niz;
    try{
        unique_lock<mutex> l(m);
        niz = new int[NUM];
    }catch(const bad_alloc& e){
    cout << "Alokacija nije uspela: "<< e.what() << endl;
    }
}

int main(){
    thread t1(kreiraj), t2(kreiraj);
    t1.join(); t2.join();

    return 0;
}

