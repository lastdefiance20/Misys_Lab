#pragma once

#include "MsgPubSub.h"
#include "Window.h"
#include "Tetris.h"
#include <unistd.h> //for random

class RandomCtrl: public Sub, public Pub {
 private:
  Window *win; // console window
  Msg key_msg; // msg to publish
  TetrisState state;
  
 public:
  RandomCtrl(Window *w, string n);
  void handle(Msg *msg);
};