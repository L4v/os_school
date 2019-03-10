#ifndef NITI_H
#define NITI_H

#include <thread>

using namespace std;

double izracunaj(list<string> kupljeno, map<string, double> cenovnik) {
    double rez;
    thread t(obracunajUkupno, kupljeno, cenovnik, ref(rez));
    t.join();
    return rez;
}

#endif // NITI_H
