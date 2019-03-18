/*
Izmeniti program data_race i realizovati zaštitu pristupa brojaču uz pomoć klase mutex i njenih operacija lock() i unlock(). Primetiti koliko sada traje izvršavanje programa.
*/

#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

mutex m;

const int ITERATOR = 0xFFFFFFF;
int brojac = 0;

void increment() {
    for(int i = 0; i < ITERATOR; i ++){
            m.lock();
            brojac++;
            m.unlock();
    }
}
void decrement() {
    for(int i = 0; i < ITERATOR; i ++){
            m.lock();
            brojac--;
            m.unlock();
    }
}

int main(){
    thread t1(increment), t2(decrement);
    t1.join(); t2.join();
    cout << brojac << endl;

    return 0;
}


