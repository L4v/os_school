/*
Napraviti konkurentni program koji u funkciji visina pita korisnika koliko je visok. Nakon toga korisnik unosi svoju visinu. Na kraju u funkciji se ispisuje uneta visina.

Kreirati 2 niti od date funkcije. Ispratiti ispis.
*/
#include <thread>
#include <iostream>

using namespace std;

void visina(){
    cout << "Kolika je Vasa visina?" << endl;
    int x;
    cin >> x;
    cout << "Vasa visina je " << x << endl;
}

int main(){
    thread t1(visina), t2(visina);
    t1.join();
    t2.join();

    return 0;
}

