#pragma once

#include "MsgPubSub.h"
#include "Window.h"

class ThreadCtrl: public Sub, public Pub {
 private:
  Window *win; // console window
  Msg msg_end; // msg to publish
  
 public:
  ThreadCtrl(Window *w, string n);
  void run();
};