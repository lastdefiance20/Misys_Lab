#pragma once

#include <unistd.h>

#include "MsgPubSub.h"
#include "Window.h"
#include <arpa/inet.h>
#include <sys/socket.h>

class SendCtrl: public Sub, public Pub {
 private:
  Window *win; // console window
  int socket; // socket number
  
 public:
  SendCtrl(Window *w, string n, int sock);
  void handle(Msg *msg);
};