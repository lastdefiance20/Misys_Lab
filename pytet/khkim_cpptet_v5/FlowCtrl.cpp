#include <FlowCtrl.h>

FlowCtrl::FlowCtrl(Window *w, string n, char c): key_msg(MSG_KEY, 0, NULL)
{
  win = w;
  name = n;
  id = c;
};

void FlowCtrl::handle(Msg *msg){
  if(msg->what == MSG_KEY){
    if(msg->key == id){
        key_msg.key = 'F';
        notifySubs(&key_msg);
    }
    else{
        key_msg.key = 'T';
        notifySubs(&key_msg);
    }
  }
  else if((msg->what == MSG_MAT)&&(TetrisState(msg->key) == Finished)){
    key_msg.key = id;
    notifySubs(&key_msg);
  }
}