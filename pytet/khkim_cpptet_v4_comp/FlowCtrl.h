#pragma once

#include "MsgPubSub.h"
#include "Window.h"
#include "Tetris.h"

class FlowCtrl: public Sub, public Pub {
 private:
  Window *win; // console window
  Msg key_msg; // msg to publish
  TetrisState state;
  char id;
  
 public:
  FlowCtrl(Window *w, string n, char c);
  void handle(Msg *msg);
};