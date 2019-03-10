#ifndef OBRACUN_H
#define OBRACUN_H

#include <string>
#include <map>
#include <list>

using namespace std;

void obracunajUkupno(list<string> kupljeno, map<string, double> cenovnik, double& ukupno) {
    ukupno = 0.;
    for(auto i : kupljeno)
        ukupno += cenovnik[i];
}

#endif // OBRACUN_H
