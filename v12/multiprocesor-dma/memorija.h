#ifndef MEMORIJA_H_INCLUDED
#define MEMORIJA_H_INCLUDED

#include <vector>
#include <mutex>;

class Memorija {
private:
    std::vector<char> memorija; // Memorija, vektor karaktera (bajtova)
    std::mutex m;

public:
    Memorija(int bajtova) {
        memorija.resize(bajtova, 0); // Alocira odredjen broj bajtova u memoriji
    }

    /*
    * Vraca vrednost sa odredjene adrese
    */
    char citaj(int adresa) {
        // Lock zbog toga da bi metoda bila thread-safe
        std::unique_lock<std::mutex> lock(m);

        // Simulira izvrsavanje citanja
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        // Vraca vrednost sa zeljene adrese
        return memorija[adresa];
    }

    /*
    * Upisuje vrednost na odredjenu adresu
    */
    void pisi(int adresa, char vrednost) {
        // Lock zbog thread-safe uslova u zadatku
        std::unique_lock<std::mutex> lock(m);

        // Simulira izvrsavanje upisa
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        // Upisuje vrednost
        memorija[adresa] = vrednost;
    }
};

#endif // MEMORIJA_H_INCLUDED
