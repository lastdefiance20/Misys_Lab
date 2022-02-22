#pragma once

#include <unistd.h>

#include "MsgPubSub.h"
#include "Window.h"
#include "Tetris.h"

class DelRectCtrl: public Sub, public Pub {
 private:
  Window *win; // console window
  Msg mat_msg; // msg to publish
  TetrisState state;
  
 public:
  DelRectCtrl(Window *w, string n);
  void handle(Msg *msg);
};