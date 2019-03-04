/*
Definisati funkciju: void increment(int& a);
koja povećava (za jedan) vrednost argumenta.

Napraviti program koji:
a) poziva funkciju increment()
b) stvara nit od funkcije increment()
*/

#include <iostream>
#include <thread>
using namespace std;

void increment(int& a){
    a++;
}

int main()
{
    int a = 1;
    cout << a << endl;
    thread t(increment, ref(a));
    t.join();
    cout << a << endl;
    return 0;
}
