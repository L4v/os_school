/*
Napraviti program koji prihvata proizvoljan broj linija sa standardnog ulaza i zatim ih ispisuje u obrnutom redosledu
(poslednja prihvaćena linija se ispisuje prva).
*/

#include <iostream>
#include <string>
#include <list>
using namespace std;

int main()
{
    list<string> linije;
    string temp;

    while(getline(cin, temp))
        linije.push_back(temp);

    for(auto it = linije.rbegin(); it != linije.rend(); it++)
        cout << *it;

    return 0;
}
