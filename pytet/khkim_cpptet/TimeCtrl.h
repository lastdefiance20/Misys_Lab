#pragma once

#include <unistd.h>

#include "MsgPubSub.h"
#include "Window.h"

class TimeCtrl: public Sub, public Pub {
 private:
  Window *win; // console window
  Msg key_msg; // msg to publish

 public:
  TimeCtrl(Window *w, string n);
  void run();
  void handle(Msg *msg);
};
