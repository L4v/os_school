/*
Napraviti program koji kreira jednu nit i u okviru niti ispisuje proizvoljnu rečenicu.
*/

#include <iostream>
#include <thread>

using namespace std;

void fun(){ cout << "Hello" << endl; }

int main()
{
    thread nit(fun);
    nit.join();
    return 0;
}
