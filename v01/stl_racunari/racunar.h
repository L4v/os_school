#ifndef RACUNAR_H_INCLUDED
#define RACUNAR_H_INCLUDED
#include <string>

using namespace std;

class Racunar {
protected:
    string hdd;
    string procesor;

public:
    Racunar(){
        hdd = "Default HDD";
        procesor = "Default CPU";
    }

    Racunar(string hdd, string procesor){
        this->hdd = hdd;
        this->procesor = procesor;
    }

    string getHDD(){return hdd;}

    string getCPU(){ return procesor; }

};

#endif // RACUNAR_H_INCLUDED
