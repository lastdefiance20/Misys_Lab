#include <Umpire.h>

Umpire::Umpire(Window *w, string n): key_msg(MSG_KEY, 0, NULL)
{
  win = w;
  name = n;
};

void Umpire::handle(Msg *msg){
  if(judged == false){
    win->printw("judge");
    key_msg.key = msg->key;
    notifySubs(&key_msg);
    judged = true;
  }
}