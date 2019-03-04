/*
Napraviti program koji kreira jednu nit kojoj se prosleđuju dva cela broja a i b. U okviru niti sabrati brojeve i ispisati na ekran njihov zbir.
*/

#include <iostream>
#include <thread>
using namespace std;

void fun(int* a, int* b){
    cout << *a + *b << endl;
}

int main()
{
    int a, b;
    cout << "Uneti brojeve a i b: " << endl;
    cin >> a;
    while(cin.fail()){
        cin.clear();
        cin.ignore(256, '\n');
        cin >> a;
    }
    cin >> b;
    while(cin.fail()){
        cin.clear();
        cin.ignore(256, '\n');
        cin >> b;
    }
    thread t(fun, &a, &b);
    t.join();
    return 0;
}
