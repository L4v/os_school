#ifndef RED_H_INCLUDED
#define RED_H_INCLUDED

#include <deque>
#include <algorithm>

#include "cv_hrono.h"

using namespace std;

class Red{
private:
    int prioritet; // Prioritet trenutnog reda
    cv_hrono* cv; // Omogucuje ciklicno kretanje
    deque<int> procesi; // Double ended queue sa id-ovima

public:
    Red(int i):prioritet(i){
        cv = new cv_hrono;
    }

    void dodaj_u_red(int id, unique_lock<mutex>& lock){
        auto it = find(procesi.begin(), procesi.end(), id);
        if(it == procesi.end())
            procesi.push_back(id);
        cv->wait(lock);
    }

    int izbaci(){
        cv->notify_one();
        int id = procesi.front();
        procesi.pop_front();
        return id;
    }

    int getPrioritet() { return prioritet; }
    bool prazan(){ return procesi.empty(); }
};

#endif // RED_H_INCLUDED
