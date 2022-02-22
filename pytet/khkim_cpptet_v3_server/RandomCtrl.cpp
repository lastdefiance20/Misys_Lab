#include <RandomCtrl.h>

RandomCtrl::RandomCtrl(Window *w, string n): key_msg(MSG_KEY, 0, NULL)
{
  win = w;
  name = n;
  srand((unsigned int)time(NULL));
};

void RandomCtrl::handle(Msg *msg){
  state = TetrisState(msg->key);
  if((state == NewBlock)||(state == NewBlockDelR)){
    key_msg.key = (char)('0' + rand() % 7);
    notifySubs(&key_msg);
  }
}