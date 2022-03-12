#include <WLCtrl.h>

WLCtrl::WLCtrl(Window *w, string n): key_msg(MSG_KEY, 0, NULL)
{
  win = w;
  name = n;
  state = Finished;
};

void WLCtrl::handle(Msg *msg){
  if(msg->key == 'T'){
    win->printw("Lose");
    key_msg.key = state;
    notifySubs(&key_msg);
  }
  else if(msg->key == 'F'){
    win->printw("Win");
    key_msg.key = state;
    notifySubs(&key_msg);
  }
}