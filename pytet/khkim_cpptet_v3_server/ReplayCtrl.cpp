#include <ReplayCtrl.h>

ReplayCtrl::ReplayCtrl(Window *w, string n): key_msg(MSG_KEY, 0, NULL)
{
  win = w;
  name = n;
  readFile.open("log.txt");
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