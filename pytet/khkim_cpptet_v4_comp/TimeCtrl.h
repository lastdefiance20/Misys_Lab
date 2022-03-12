#pragma once

#include <unistd.h>

#include "MsgPubSub.h"
#include "Window.h"

class TimeCtrl: public Sub, public Pub {
 private:
  Window *win; // console window
  Msg key_msg; // msg to publish
  float time; // time to publish(sec)

 public:
  TimeCtrl(Window *w, string n, float t);
  void run();
  void handle(Msg *msg);
};
