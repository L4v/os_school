/*
Napisati program koji omogućava evidentiranje fakultetskih učionica.

Za svaku učionicu potrebno je evidentirati naziv učionice, kvadraturu i sve računare koji se nalaze u učionici.
Za svaki računar evidentiraju se podaci o hard disku i procesoru.

U okviru main funkcije, definisati jednu učionicu koja sadrži dva računara.
Zatim je potrebno na konzolu ispisati hard disk i procesor svakog računara u učionici.
*/

#include <iostream>
#include "ucionica.h"

using namespace std;

int main()
{
    Ucionica u;
    Racunar r1, r2("hard", "cpu");
    cout << r1.getHDD() << " " << r1.getCPU() << endl;
    cout << r2.getHDD() << " " << r2.getCPU() << endl;
    u.add(r1); u.add(r2);
    list<Racunar> temp_list = u.getRacunari();
    list<Racunar>::iterator it;
    for(it = temp_list.begin(); it != temp_list.end(); it++)
        cout << it->getCPU() << " " << it->getHDD() << endl;

    return 0;
}
