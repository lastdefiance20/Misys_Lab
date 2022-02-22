#include <LogCtrl.h>

LogCtrl::LogCtrl(Window *w, string n): key_msg(MSG_KEY, 0, NULL)
{
  win = w;
  name = n;
  writeFile.open("log.txt"); //reset log.txt file
  writeFile.close();
};

void LogCtrl::handle(Msg *msg){
  writeFile.open("log.txt", ofstream::app); //app -> appending to its existing contents.
  writeFile<<msg->key;
  writeFile.close();

  key_msg.key = msg->key;
  notifySubs(&key_msg);
}