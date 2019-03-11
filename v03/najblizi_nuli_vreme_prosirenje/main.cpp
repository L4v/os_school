/*
Proširiti zadatak najbilzi_nuli_vreme ispisom u sekundama i minutima.
*/


#include <iostream>
#include <thread>
#include <vector>
#include <cmath>

using namespace std;
using namespace chrono;

typedef vector<double>::const_iterator cit;

struct vreme {
    system_clock::time_point pocetak;
    system_clock::time_point kraj;
};

void f(cit pocetak, cit kraj, double & min, struct vreme * v) {
    v->pocetak = system_clock::now();
    min = *pocetak;
    for(auto it = pocetak +1; it != kraj; it++){
        if(abs(*it) < abs(min))
            min = *it;
    }
    v->kraj = system_clock::now();
}

const int BROJ_NITI = 3;
const int DUZINA = 900000;

int main() {
    int SEGMENT = DUZINA / BROJ_NITI;

    srand(time(NULL));   // Postavljamo seme random generatora na trenutno vreme, što dovodi do kvalitetnijeg izbora slučajnih brojeva.

    vector<double> v(DUZINA);
    // Puni se vektor pseudo-slučajnim brojevima
    for (int i = 0; i < DUZINA; i++)
        v[i] = rand();

    double minimumi[BROJ_NITI];  // Niz elemenata najbližih nuli - svaka programska nit će dati svoj međurezltat
    struct vreme vremena[BROJ_NITI]; // Niz struktura u koji će biti upisani podaci o trajanju izvršavanja svake niti
    cit it = v.begin();
    thread niti[BROJ_NITI];

    for(int i = 0; i < BROJ_NITI; i ++){
        if(i == BROJ_NITI - 1){
            niti[i] = thread(f, it, v.end(), ref(minimumi[i]), &vremena[i]);
            break;
        }
        niti[i] = thread(f, it, it + SEGMENT, ref(minimumi[i]), &vremena[i]);
        it += SEGMENT;
    }

    for(int i = 0; i < BROJ_NITI; i ++)
        niti[i].join();

    for(int i = 0; i < BROJ_NITI; i++){
        duration<double, milli> trajanjems = vremena[i].kraj - vremena[i].pocetak;
        duration<double> trajanjes = trajanjems / 1000;
        duration<double, ratio<60>> trajanjemin = trajanjems / 60000;
        cout << "Nit " << i+1 << " se izvrsavala " << trajanjemin.count() << " minuta" << " tj " << trajanjes.count() << " sekundi" << endl;
    }

}

