/*
Uneti 5 celih brojeva sa standardnog ulaza.
Korišćenjem STL algoritma sortirati brojeve u opadajućem redosledu.
*/

#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

// Sluzi kao kriterijum za sortiranje
bool crit(int x, int y){ return (x > y); }

vector<int> sort_list(const vector<int>& numbers){
    vector<int> nums = numbers;
    cout << endl << "Sortira se..." << endl;
    sort(nums.begin(), nums.end(), crit);
    cout << "Sortirano..." << endl;
    return nums;
}

int main()
{
    vector<int> numbers;
    int x, i = 0;
    cout << "Poceti unos: " << endl;
    while(i < 5){
        cin >> x;
        if(cin.fail()){
            cout << "Nije broj, probati opet: " << endl;
            cin.clear();
            cin.ignore(256, '\n');
            cin >> x;
            continue;
        }
        numbers.push_back(x);
        i++;
    }

    cout << "Pre sortiranja: ";
    for(auto i : numbers)
        cout << i << " ";
    cout << endl << "Posle sortiranja: ";
    numbers = sort_list(numbers);
    for(auto i : numbers)
        cout << i << " ";
    cout << endl;

    return 0;
}
