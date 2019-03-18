/*
Napraviti konkurentni program koji modeluje klasu brojača. Interfejs klase sadrži sledeće metode:

class Brojac {
   public:
      void inc();
      void dec();
      friend ostream& operator<<(ostream& , Brojac& );
};

Metode inc i dec povećavaću i smanjuju vrednost brojača respektivno. Operator << služi za ispis brojača na ekran.

Klasa treba da zaštiti konzistentnost brojača u datim metodama uz pomoć klasa mutex i unique_lock.

Kreirati 1 globalni objekat brojača kome će pristupati 2 niti.

Kreirati 2 niti pri čemu jedna nit poziva metodu uvećavanja brojača 1000000 puta a druga metodu smanjivanja brojača 1000000 puta. Na kraju programa ispisati konačnu vrednost brojača nakon uvećavanja i smanjivanja.
*/

#include <iostream>
#include <thread>
#include <mutex>

using namespace std;


class Brojac {
private:
    mutex m;
    int x;
   public:
      Brojac() : x(0){}
      void inc(){
        unique_lock<mutex> lock(m);
        x++;
      }
      void dec(){
        unique_lock<mutex> lock(m);
        x--;
      }
      friend ostream& operator<<(ostream& o, Brojac& b){
        o << "Brojac: " << b.x << endl;
        return o;
      }
};

Brojac brojac;
void f1(){
    for(int i = 0; i < 1000000; i ++)
        brojac.inc();
}

void f2(){
    for(int i = 0; i < 1000000; i++)
        brojac.dec();
}

int main(){
    thread t1(f1), t2(f2);
    t1.join(); t2.join();
    cout << brojac;
    return 0;
}
