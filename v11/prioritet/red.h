#ifndef RED_H_INCLUDED
#define RED_H_INCLUDED

#include "cv_hrono.h"
#include <deque>

class Red{
private:
    int prioritet; // Prioritet trenutnog reda
    cv_hrono cv; // Omogucuje ciklicno kretanje
    deque<int> procesi; // Double ended queue sa id-ovima
};

#endif // RED_H_INCLUDED
