/*
Napisati konkurentni program koji stvara 2 niti od funkcije f. Funkcija f treba da prolazi kroz petlju dužine 10000 elemenata i ispisuje parne ili neparne brojeve (među tih 10000 elemenata).

Ispis parnih ili neparnih brojeva se vrši pozivom metoda ispisi_parne i ispisi_neparne. U metodi ispisi_parne prvo se pre ispisa zaključava muteks m1, a potom muteks m2. U metodi ispisi_neparne prvo se pre ispisa zaključava muteks m2, a potom muteks m1.

Sva zaključavanja se vrše kroz objekte klase unique_lock.

Pratiti izvršavanje programa.
*/


#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

mutex m1, m2;

const int SIZE = 10000;

void ispisi_parne(int *x){
    unique_lock<mutex> l1(m1);
    unique_lock<mutex> l2(m2);
    cout << "PARNO: " << *x << endl;
}

void ispisi_neparne(int *x){
    unique_lock<mutex> l1(m2);
    unique_lock<mutex> l2(m1);
    cout << "NEPARNO: " << *x << endl;
    }

void f(int* x){
    for(int i = 0; i < SIZE; i ++)
        x[i] % 2 == 0 ? ispisi_parne(&x[i]) : ispisi_neparne(&x[i]);
}

int main(){
    int* niz = new int[SIZE];
    for(int i = 0; i < SIZE; i++)
        niz[i] = rand()%64 + 1;
    thread t1(f, niz), t2(f, niz);
    t1.join(); t2.join();
    return 0;
}
