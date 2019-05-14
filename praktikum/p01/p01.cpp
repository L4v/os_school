#include <mutex>
#include <thread>
#include <iostream>

using namespace std;
using namespace chrono;
using namespace this_thread;

#include "box.hh"

Message_box<int> box;

const int TERMINATION = -1;
const int USER_INTERVAL = 1;
const int SERVER_INTERVAL = 2;

void thread_user(){
    int request_time = 0;
    int time_limit = 3;
    cout << endl << "USER-SERVER SIMULATION" << endl;
    while(request_time < time_limit) {
	box.send(&request_time);
	request_time += USER_INTERVAL;
    }
    box.send(&TERMINATION);
}

void
thread_server()
{
int service_end_time = 0;
int new_request_time;
int request_count = 0;
int mean_waiting_time = 0;
while((new_request_time = box.receive()) != TERMINATION) {
request_count++;
if(new_request_time < service_end_time)
mean_waiting_time += service_end_time - new_request_time;
else
service_end_time = new_request_time;
service_end_time += SERVER_INTERVAL;
}
mean_waiting_time /= request_count;
cout << endl << "mean waiting time = " << mean_waiting_time << '\n';
}

int
main()
{
thread user(thread_user);
thread server(thread_server);
user.join();
server.join();
}