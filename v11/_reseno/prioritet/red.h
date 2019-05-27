	#ifndef RED_H_INCLUDED
#define RED_H_INCLUDED

#include <deque>
#include <algorithm>

#include "cv_hrono.h"

using namespace std;

class Red{
private:
    int prioritet;
    cv_hrono* cv;
    deque<int> spremni;
public:
    Red(int p): prioritet(p){
        cv = new cv_hrono;
    }

    void dodaj_u_red(int id_procesa, unique_lock<mutex>& lock){
        auto it = find(spremni.begin(), spremni.end(), id_procesa);
        if(it == spremni.end()){
            spremni.push_back(id_procesa);
        }
        cv->wait(lock);
    }

    int izbaci_iz_reda(){
        cv->notify_one();

        int id_procesa = spremni.front();
        spremni.pop_front();

        return id_procesa;
    }

    bool prazan(){ return spremni.empty(); }

    int preuzmi_prioritet(){ return prioritet; }

};

#endif // RED_H_INCLUDED
