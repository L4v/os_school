/*
Napraviti program koji prihvata proizvoljan broj linija sa standardnog ulaza i zatim ih ispisuje u redosledu kojim su uneti.
*/

#include <iostream>
#include <string>
#include <list>

using namespace std;

int main()
{
    list<string> lines;
    list<string>::iterator it;
    string line;
    cout << "Poceti unos:" << endl;
    while(getline(cin, line))
        lines.push_back(line);

    cout << "Unesene linije: " << endl;
    for(auto l : lines)
        cout << l << endl;

    /*
    for(it = lines.begin(); it != lines.end(); it ++)
        cout << *it << endl;
    */
    return 0;
}
