/*
Napraviti program koji prihvata proizvoljan broj linija sa standardnog ulaza i zatim ih ispisuje u redosledu kojim su uneti.
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
    for(auto i : linije)
        cout << i;
    return 0;
}
