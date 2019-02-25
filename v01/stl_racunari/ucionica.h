#ifndef UCIONICA_H_INCLUDED
#define UCIONICA_H_INCLUDED

#include <list>
#include "racunar.h"

class Ucionica{
private:
    string naziv;
    int kvadratura;
    list<Racunar> racunari;


public:
    Ucionica(){
        naziv = "Basic ucionica";
        kvadratura = 1;
    }

    Ucionica(string naziv, int kvadratura){
        this->naziv = naziv;
        this->kvadratura = kvadratura;
    }

    void add(Racunar& r){
        racunari.push_back(r);
    }

    void remove(){
        racunari.pop_back();
    }

    list<Racunar> getRacunari(){ return this->racunari;}

    int getKvadratura(){return this->kvadratura;}
    string getNaziv(){ return this->naziv;}

    void setKvadratura(int k){this->kvadratura = k;}
    void setNaziv(string n){this->naziv = n;}

};

#endif // UCIONICA_H_INCLUDED
