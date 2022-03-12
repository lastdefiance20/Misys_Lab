#pragma once

#include "MsgPubSub.h"
#include "Window.h"
#include "CTetris.h"

class Model: public Sub, public Pub {
 private:
  Window *win; // console window
  Msg mat_msg; // msg with a matrix

  CTetris *board;
  TetrisState state;
  char key;

 public:
  Model(Window *w, string n);
  void handle(Msg *msg);
};
