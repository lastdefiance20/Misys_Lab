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
  
 public:
  LogCtrl(Window *w, string n);
  void handle(Msg *msg);
};