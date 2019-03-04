#ifndef SUMA_VEKTORA_H_INCLUDED
#define SUMA_VEKTORA_H_INCLUDED

#include <vector>

using namespace std;

typedef vector<double>::const_iterator ci;

/*
Funkcija niti. Funkcija preuzima sledeće parametre:
1) ci a_begin -> iterator na početak dela vektora a
2) ci a_end   -> iterator na kraj dela vektora a (dovoljan je samo jedan kraj)
3) ci b_begin -> iterator na početak dela vektora b
4) vector<double>::iterator sum_begin  -> iterator na početak dela vektora suma
*/
void f(ci a_begin, ci a_end, ci b_begin, vector<double>::iterator sum_begin) {
    ci ita, itb;
    vector<double>::iterator it;
    for(ita = a_begin, itb = b_begin, it = sum_begin; ita != a_end; ita ++, itb ++, sum_begin++){
        *it = *ita + *itb;
    }
}

#endif // SUMA_VEKTORA_H_INCLUDED
