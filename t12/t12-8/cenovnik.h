#ifndef CENOVNIK_H
#define CENOVNIK_H

#include <string>
#include <map>

using namespace std;

void nadjiPovoljno(map<string, double> a, map<string, double> b, map<string, double> &jeftino) {
    map<string, double>::const_iterator ita, itb;
    for(ita = a.begin(), itb = b.begin(); ita != a.end(); ita++, itb++)
    jeftino.insert({ita->first, ita->second < itb->second ? ita->second : itb->second});
}

#endif // CENOVNIK_H
