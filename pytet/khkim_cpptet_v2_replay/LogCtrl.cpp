#include <LogCtrl.h>

LogCtrl::LogCtrl(Window *w, string n): key_msg(MSG_KEY, 0, NULL)
{
  win = w;
  name = n;
  writeFile.open("log.txt");
  writeFile.close();
};

void LogCtrl::handle(Msg *msg){
  writeFile.open("log.txt", ofstream::app);

  writeFile<<msg->key;

  writeFile.close();
}