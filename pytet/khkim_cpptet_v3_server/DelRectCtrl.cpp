#include <DelRectCtrl.h>

DelRectCtrl::DelRectCtrl(Window *w, string n): mat_msg(MSG_MAT, 0, NULL)
{
  win = w;
  name = n;
};

void DelRectCtrl::handle(Msg *msg){
  state = TetrisState(msg->key);
  if(state == NewBlockDelR){
    mat_msg.mat = msg->mat;
    notifySubs(&mat_msg);
  }
}