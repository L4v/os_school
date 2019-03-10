#ifndef OBRTANJE_H
#define OBRTANJE_H

#include <list>

using namespace std;

// Implementacija obrtanja redosleda elemenata u listi
// Ova funkcija treba da bude iskorišćena za funkciju niti koje ovaj program pokreće.
//
// ulazna  - Ulazna lista, koja se obrađuje
// izlazna - Rezultujuća lista (biće popunjena elementima u obrnutom redosledu)
//
void obrniRedosled(const list<int>& ulazna, list<int>& izlazna) {
    list<int>::const_reverse_iterator itu;
    for(itu = ulazna.rbegin(); itu != ulazna.rend(); itu++)
        izlazna.push_back(*itu);
}

#endif // OBRTANJE_H
