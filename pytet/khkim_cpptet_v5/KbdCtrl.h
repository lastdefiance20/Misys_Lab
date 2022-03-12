#pragma once

#include <unistd.h>
#include <map>

#include "MsgPubSub.h"
#include "Window.h"

class KbdCtrl: public Sub, public Pub {
 private:
  Window *win; // console window
  Msg key_msg; // msg to publish
  map<char, char> key_set;
  bool isStdinReadable();
  char getChar(); 

 public:
  KbdCtrl(Window *w, string n, map<char, char> m);
  void run();
  void handle(Msg *msg);
};
