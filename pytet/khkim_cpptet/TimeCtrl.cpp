#include "TimeCtrl.h"

TimeCtrl::TimeCtrl(Window *w, string n): key_msg(MSG_KEY, 's', NULL)
{
  win = w;
  name = n;
};

//Msg key_msg(MSG_KEY, 's', NULL);

void TimeCtrl::handle(Msg *msg)
{
  //win->printw(name + ".handle() called.\n");
  sleep(1);
  notifySubs(&key_msg);
}

void TimeCtrl::run() 
{
  //win->printw(name + ".run() begins.\n");
  Msg *msg;
  while (true) {
    mtx.lock();
    if (que.empty()) 
      msg = NULL;
    else {
      msg = que.front();
      que.pop();
    }
    mtx.unlock();
    //win->printw(name + ".run() wakes up.\n");

	  if (msg && msg->what == MSG_END)
	    break;

    handle(msg);
  }
  //win->printw(name + ".run() ends.\n");
};
