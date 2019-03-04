/*
Napraviti konkurentni program koji pokreće više niti u petlji. Svakoj niti proslediti njen redni broj prilikom kreiranja i svaka nit treba da ispiše sopstveni redni broj u okviru tela niti.
*/

#include <iostream>
#include <thread>

using namespace std;

void fun(int x){
    cout << "Ja sam nit: " << x << endl;
}

const int MAX_NITI = 10;

int main()
{
    thread t[MAX_NITI];
    for(int i = 0; i < MAX_NITI; i++)
        t[i] = thread(fun, i);
    for(int i = 0; i < MAX_NITI; i ++)
        t[i].join();
    return 0;
}
