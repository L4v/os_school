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
    map<string, string> recnik;
    recnik["white"] = "bela";
    recnik["red"] = "crvena";
    recnik["black"] = "crna";
    string rec;
    while(getline(cin, rec))
        cout << "Prevod: " << (recnik[rec] != "" ? recnik[rec] : "rec ne postoji.") << endl;
    return 0;
}
