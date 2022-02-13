#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

#include "MsgPubSub.h"
#include "Window.h"
#include "Matrix.h"

class View: public Sub {
 private:
  Window *win;     // console window
  Window *mwin;    // main window
  
 public:
  View(Window *w_main, Window *w_con, string n);
  void handle(Msg *msg);
};
