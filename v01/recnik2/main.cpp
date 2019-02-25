/*
Napraviti jednostavan program za prevođenje reči sa engleskog na srpski jezik.
Nakon unosa reči sa standardnog ulaza, ispisuje se prevod (ukoliko unesena reč postoji u rečniku).

Uneti 3 engleske reči i njihove prevode na srpski.
*/

#include <iostream>
#include <string>
#include <map>

using namespace std;

int main()
{
    cout << "TEST";
    map<string, string> recnik;
    recnik["black"] = "crna";
    recnik["white"] = "bela";
    recnik["red"] = "crvena";
    string eng_rec;
    cout << "TEST";
    while(getline(cin, eng_rec))
        cout << "Prevod: " << recnik[eng_rec] << endl;
    return 0;
}
