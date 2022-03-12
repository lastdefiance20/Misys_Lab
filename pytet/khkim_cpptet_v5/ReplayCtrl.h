#pragma once

#include <unistd.h>

#include "MsgPubSub.h"
#include "Window.h"
#include <fstream>

class ReplayCtrl: public Sub, public Pub {
 private:
  Window *win; // console window
  Msg key_msg; // msg to publish
  ifstream readFile;
  string txtname;
  
 public:
  ReplayCtrl(Window *w, string n, string t);
  void handle(Msg *msg);
};