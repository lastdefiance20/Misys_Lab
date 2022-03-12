#pragma once

#include <unistd.h>

#include "MsgPubSub.h"
#include "Window.h"
#include <fstream>

class LogCtrl: public Sub, public Pub {
 private:
  Window *win; // console window
  Msg key_msg; // msg to publish
  ofstream writeFile;
  string txtname;
  
 public:
  LogCtrl(Window *w, string n, string t);
  void handle(Msg *msg);
};