#include <ThreadCtrl.h>

ThreadCtrl::ThreadCtrl(Window *w, string n): msg_end(MSG_END, 0, NULL)
{
  win = w;
  name = n;
};

void ThreadCtrl::handle(Msg *msg){
  state = TetrisState(msg->key);
  if(state == Finished){
    win->printw("\nover");
    notifySubs(&msg_end);
  }
}