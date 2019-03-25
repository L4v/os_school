#ifndef KOORDINATOR_H_INCLUDED
#define KOORDINATOR_H_INCLUDED

#include <mutex>
#include <condition_variable>

using namespace std;

class Koordinator {
private:
    const int cekam_niti;     // Konstanta koja govori koliko niti se čeka na završetak pre nego što se aktivira poslednja.
    condition_variable free;
    int done;
    mutex m;
public:
    Koordinator(int cn) : cekam_niti(cn), done(cn) {
        // Proširiti po potrebi ...
    }

    // Metoda koju na svom KRAJU zovu niti (a i b) koje ne čekaju (izvršavaju se odmah).
    void zavrsio() {
        unique_lock<mutex> lock(m);
        done --;
        free.notify_one();
    }

    // Metodu koju na svom POČETKU zovu niti (c) koje čekaju (ne izvršavaju se odmah).
    void cekam() {
        unique_lock<mutex> lock(m);
        while(done != 0)
            free.wait(lock);

    }
};

#endif // KOORDINATOR_H_INCLUDED
