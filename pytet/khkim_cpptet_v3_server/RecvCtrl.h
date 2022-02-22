#pragma once

#include <unistd.h>

#include "MsgPubSub.h"
#include "Window.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

class RecvCtrl: public Sub, public Pub {
 private:
  Window *win; // console window
  Msg key_msg; // msg to publish
  int socket; // socket number
  char r_buff[256];
  
 public:
  RecvCtrl(Window *w, string n, int sock);
  void handle(Msg *msg);
  void run();
};