#include <LogCtrl.h>

LogCtrl::LogCtrl(Window *w, string n, string t): key_msg(MSG_KEY, 0, NULL)
{
  win = w;
  name = n;
  txtname = t;
  writeFile.open(txtname); //reset txt file
  writeFile.close();
};

void LogCtrl::handle(Msg *msg){
  writeFile.open(txtname, ofstream::app); //app -> appending to its existing contents.
  writeFile<<msg->key;
  writeFile.close();

  key_msg.key = msg->key;
  notifySubs(&key_msg);
}