#include <ReplayCtrl.h>

ReplayCtrl::ReplayCtrl(Window *w, string n, string t): key_msg(MSG_KEY, 0, NULL)
{
  win = w;
  name = n;
  txtname = t;
  readFile.open(txtname);
};

void ReplayCtrl::handle(Msg *msg){
  if(readFile.is_open()){
    readFile.get(key_msg.key);
    notifySubs(&key_msg);

    if(readFile.eof()){
        readFile.close();  
    }
  }
}