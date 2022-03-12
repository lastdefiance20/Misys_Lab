#pragma once

#include <unistd.h>

#include "MsgPubSub.h"
#include "Window.h"

class IdCtrl: public Sub, public Pub {
 private:
  Window *win; // console window
  Msg key_msg; // msg to publish
  char id;
  bool blocking;
  
 public:
  IdCtrl(Window *w, string n, char t, bool b);
  void handle(Msg *msg);
};