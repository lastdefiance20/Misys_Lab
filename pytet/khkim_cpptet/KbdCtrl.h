#pragma once

#include <unistd.h>

#include "MsgPubSub.h"
#include "Window.h"

class KbdCtrl: public Sub, public Pub {
 private:
  Window *win; // console window
  Msg key_msg; // msg to publish
  bool isStdinReadable();
  char getChar(); 

 public:
  KbdCtrl(Window *w, string n);
  void run();
  void handle(Msg *msg);
};
