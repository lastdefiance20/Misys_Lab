#include <ThreadCtrl.h>

ThreadCtrl::ThreadCtrl(Window *w, string n): msg_end(MSG_END, 0, NULL)
{
  win = w;
  name = n;
};

void ThreadCtrl::run() 
{
while (true) {
    unique_lock<mutex> uqlck(mtx);
    if (que.empty())
    cv.wait(uqlck, [&]{ return !que.empty(); } );
    
    Msg *msg = que.front();
    que.pop();
    uqlck.unlock();
    //win->printw(name + ".run() wakes up.\n");

    if (msg->what == MSG_END){
        notifySubs(&msg_end);
        break;
        }
    }
  //win->printw(name + ".run() ends.\n");
};