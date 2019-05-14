#include<thread>
#include<iostream>
#include<mutex>
#include<condition_variable>
using namespace std;
using namespace chrono;
using namespace this_thread;
int
mod5(int a)
{
return (a > 4 ? 0 : a);
}
enum
Philosopher_state {THINKING = 'T',
WAITING_LEFT_FORK = 'L',
HOLDING_ONE_FORK = 'O',
WAITING_RIGHT_FORK = 'R',
EATING = 'E'};
enum
Fork_state {FREE, BUSY};
class Dining_table {
mutex mx;
int philosopher_identity;
Philosopher_state philosopher_state[5];
Fork_state fork_state[5];
condition_variable fork_available[5];
void show();
public:
Dining_table();
int take_identity();
void take_fork(int fork, int philosopher,
Philosopher_state waiting_state,
Philosopher_state next_state);
void release_fork(int fork, int philosopher,
Philosopher_state next_state);
};
Dining_table::Dining_table()
{
philosopher_identity=0;
for(int i = 0; i < 5; i++) {
philosopher_state[i] = THINKING;
fork_state[i] = FREE;
}
}
    void Dining_table::show()
    {
        for(int i = 0; i < 5; i++) {
            cout << '(' << (char)(i+'0') << ':'
            << (char)philosopher_state[i] << ") ";
        }
        cout << endl;
    }
    int Dining_table::take_identity()
    {
        unique_lock<mutex> lock(mx);
        return philosopher_identity++;
    }
    void Dining_table::take_fork(int fork, int philosopher,
    Philosopher_state waiting_state,
    Philosopher_state next_state)
    {
        unique_lock<mutex> lock(mx);
        if(fork_state[fork] == BUSY) {
            philosopher_state[philosopher] = waiting_state;
            show();
            do {fork_available[fork].wait(lock);} while(fork_state[fork] == BUSY);
        }
        fork_state[fork] = BUSY;
        philosopher_state[philosopher] = next_state;
        show();
    }
    void Dining_table::release_fork(int fork, int philosopher,
    Philosopher_state next_state)
    {
        unique_lock<mutex> lock(mx);
        fork_state[fork] = FREE;
        philosopher_state[philosopher] = next_state;
        show();
        fork_available[fork].notify_one();
    }
Dining_table
dining_table;
const milliseconds
THINKING_PERIOD(10);
const milliseconds
MEANTIME(5);
const milliseconds
EATING_PERIOD(10);

void thread_philosopher()
    {
    int philosopher = dining_table.take_identity();
    int fork = philosopher;
    for(;;) {
        sleep_for(THINKING_PERIOD);
        dining_table.take_fork(fork, philosopher,
        WAITING_LEFT_FORK, HOLDING_ONE_FORK);
        sleep_for(MEANTIME);
        dining_table.take_fork(mod5(fork+1), philosopher,
        WAITING_RIGHT_FORK, EATING);
        sleep_for(EATING_PERIOD);
        dining_table.release_fork(fork, philosopher, HOLDING_ONE_FORK);
        sleep_for(MEANTIME);
        dining_table.release_fork(mod5(fork+1), philosopher, THINKING);
        }
    }
int main()
{
    cout << endl << "DINING PHILOSOPHERS" << endl;
    thread philosopher0(thread_philosopher);
    thread philosopher1(thread_philosopher);
    thread philosopher2(thread_philosopher);
    thread philosopher3(thread_philosopher);
    thread philosopher4(thread_philosopher);
    philosopher0.join();
    philosopher1.join();
    philosopher2.join();
    philosopher3.join();
    philosopher4.join();
}
