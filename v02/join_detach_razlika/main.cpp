/*
Napraviti konkurentni program koji stvara nit iz koje pita korisnika za visinu u centimetrima i potom ispisuje: “Vasa visina je <uneta_visina> cm.”

Testirati program tako što će main nit, koristeći objekat koji predstavlja novu nit, da pozove metodu join. Potom promeniti taj poziv u poziv metode detach.
*/

#include <iostream>
#include <thread>

using namespace std;

void fun(int* x){
    cout << "Unesite vasu visinu: " << endl;
    cin >> *x;
}

int main()
{
    int x;
    thread nit(fun, &x);
    nit.join();
    //nit.detach();
    cout << "Vasa visina je: " << x << endl;
    return 0;
}



