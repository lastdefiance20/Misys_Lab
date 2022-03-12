#pragma once

#include "MsgPubSub.h"
#include "Window.h"
#include "Tetris.h"

class Umpire: public Sub, public Pub {
 private:
  Window *win; // console window
  Msg key_msg; // msg to publish
  TetrisState state;
  bool judged = false;
  
 public:
  Umpire(Window *w, string n);
  void handle(Msg *msg);
};