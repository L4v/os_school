#ifndef MEMORIJA_H_INCLUDED
#define MEMORIJA_H_INCLUDED

#include <vector>
#include <mutex>;

class Memorija {
private:
    std::vector<char> memorija;
    std::mutex m;

public:
    Memorija(int bajtova) {
        memorija.resize(bajtova, 0);
    }

    char citaj(int adresa) {
        std::unique_lock<std::mutex> lock(m);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        return memorija[adresa];
    }

    void pisi(int adresa, char vrednost) {
        std::unique_lock<std::mutex> lock(m);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        memorija[adresa] = vrednost;
    }
};

#endif // MEMORIJA_H_INCLUDED
