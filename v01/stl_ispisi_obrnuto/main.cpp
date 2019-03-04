/*
Napraviti program koji prihvata proizvoljan broj linija sa standardnog ulaza i zatim ih ispisuje u obrnutom redosledu
(poslednja prihvaćena linija se ispisuje prva).
*/

#include <iostream>
#include <list>

using namespace std;

int main()
{
    list<string> lines;
    list<string>::reverse_iterator it;
    string line;
    cout << "Poceti unos:" << endl;
    while(getline(cin, line))
        lines.push_back(line);
    cout << "Unesene linije u obrnutom ispisu:" << endl;
    for(it = lines.rbegin(); it != lines.rend(); it ++)
        cout << *it << endl;


    return 0;
}
