#ifndef KOORDINATOR_H_INCLUDED
#define KOORDINATOR_H_INCLUDED

#include <condition_variable>
#include <mutex>

using namespace std;

class Koordinator {
private:
    condition_variable free;
    mutex m;
    enum State{A, B};
    State state;

public:
    Koordinator() : state(A){};

    // Funkcija se poziva nakon što je prva nit završila svoj posao, kako bi signalizirala pokretanje druge niti.
    void prvi_zavrsio() {
        unique_lock<mutex> lock(m);
        state = B;
        free.notify_one();
    }

    // Blokirajuća funkcija koja se poziva na početku druge niti kako bi nit čekala dok se prva nit ne završi.
    void drugi_ceka() {
        unique_lock<mutex> lock(m);
        while(state == A)
            free.wait(lock);
    }
};

#endif // KOORDINATOR_H_INCLUDED
