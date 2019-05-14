#include <mutex>
#include <condition_variable>

template<class MESSAGE>
class Message_box {
mutex mx;
enum Message_box_states { EMPTY, FULL };
MESSAGE content;
Message_box_states state;
condition_variable full;
condition_variable empty;
public:
Message_box() : state(EMPTY) {};
void send(const MESSAGE* message);
MESSAGE receive();
};
template<class MESSAGE>
void
Message_box<MESSAGE>::send(const MESSAGE* message)
{
unique_lock<mutex> lock(mx);
while(state == FULL)
empty.wait(lock);
content = *message;
state = FULL;
full.notify_one();
}
template<class MESSAGE>
MESSAGE
Message_box<MESSAGE>::receive()
{
unique_lock<mutex> lock(mx);
while(state == EMPTY)
full.wait(lock);
state = EMPTY;
empty.notify_one();
return content;
}