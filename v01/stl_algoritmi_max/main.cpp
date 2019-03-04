/*
Korisnik treba da unese 5 celih brojeva koristeći tastaturu (standardni ulaz).
Korišćenjem STL algoritma, među tih 5 brojeva potrebno je pronaći najveći.
*/

#include <iostream>
#include <list>
#include <algorithm>
using namespace std;

int max_int(list<int> numbers){
    list<int> nums = numbers;
    list<int>::iterator it;
    int maxnum = *nums.begin();
    for(it = nums.begin(); it != nums.end(); it ++)
        maxnum = (*it > maxnum ? *it : maxnum);
    return maxnum;
}

int main()
{
    int i = 0, x;
    list<int> nums;
    while(i < 5){
        cin >> x;
        if(cin.fail()){
            cout << "Nije broj, probajte opet: " << endl;
            cin.clear();
            std::cin.ignore(256,'\n');
            continue;
        }

        nums.push_back(x);
        i ++;
    }

    cout << "Max element je: " << max_int(nums);

    return 0;
}
