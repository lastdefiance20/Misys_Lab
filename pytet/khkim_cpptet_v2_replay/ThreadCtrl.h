#pragma once

#include "MsgPubSub.h"
#include "Window.h"
#include "Tetris.h"

class ThreadCtrl: public Sub, public Pub {
 private:
  Window *win; // console window
  Msg msg_end; // msg to publish
  TetrisState state;
  
 public:
  ThreadCtrl(Window *w, string n);
  void handle(Msg *msg);
};