/*
Napisati program koji omogućava evidentiranje fakultetskih učionica.

Za svaku učionicu potrebno je evidentirati naziv učionice, kvadraturu i sve računare koji se nalaze u učionici.
Za svaki računar evidentiraju se podaci o hard disku i procesoru.

U okviru main funkcije, definisati jednu učionicu koja sadrži dva računara.
Zatim je potrebno na konzolu ispisati hard disk i procesor svakog računara u učionici.
*/

#include <iostream>
#include <string>
#include <list>

using namespace std;
class Racunar{
private:
    string hdd, cpu;
public:
    Racunar(string hdd = "default hdd", string cpu = "default cpu"){
        this->hdd = hdd;
        this->cpu = cpu;
    }
    void setHDD(string hdd){ this->hdd = hdd; }
    void setCPU(string cpu){ this->cpu = cpu; }
    const string getHDD() const { return this->hdd; }
    const string getCPU() const { return this->cpu; }
    friend ostream& operator<<(ostream& os, const Racunar& r){
        return os << "HDD: " << r.hdd << " CPU: " << r.cpu << endl;
    }
};

class Ucionica{
private:
    list<Racunar> racunari;
    string naziv;
    int kvadratura;
public:
    Ucionica(string naziv = "default naziv", int kvadratura = 1){
        this->naziv = naziv;
        this->kvadratura = kvadratura;
    }
    void add(const Racunar& r){ this->racunari.push_back(r); }
    void remove(){ this->racunari.pop_back(); }
    friend ostream& operator<<(ostream& os, const Ucionica& u){ os << "Naziv: " << u.naziv << " Kvadratura: " << u.kvadratura << endl
        << "Racunari: ";
            for(auto i : u.racunari)
                os << i << " ";
            return os;
    }
};


int main()
{
    Ucionica ucionica("Whatever A3-4");
    Racunar r1, r2("R2 HDD", "R2 CPU"), r3("R3 HDD", "R3 CPU");
    cout << ucionica;
    ucionica.add(r1); ucionica.add(r2); ucionica.add(r3);
    cout << ucionica;
    ucionica.remove();
    cout << ucionica;

    return 0;
}
