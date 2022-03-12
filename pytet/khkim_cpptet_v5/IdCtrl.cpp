#include <IdCtrl.h>

IdCtrl::IdCtrl(Window *w, string n, char t, bool b): key_msg(MSG_KEY, 0, NULL)
{
  win = w;
  name = n;
  id = t;
  blocking = b;
};

void IdCtrl::handle(Msg *msg){
  if(blocking == true){
    if(msg->key == id){
        key_msg.key = id;
        notifySubs(&key_msg);
    }
  }
  else{
    key_msg.key = id;
    notifySubs(&key_msg);
  }
}