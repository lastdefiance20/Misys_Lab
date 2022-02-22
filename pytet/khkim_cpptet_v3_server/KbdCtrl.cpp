#include "KbdCtrl.h"

KbdCtrl::KbdCtrl(Window *w, string n): key_msg(MSG_KEY, 0, NULL)
{
  win = w;
  name = n;
}

bool KbdCtrl::isStdinReadable(void)
{
  fd_set rdfs;
  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;

  FD_ZERO(&rdfs);
  FD_SET(STDIN_FILENO, &rdfs);
  if (select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv) < 0)
    win->printw(name + ": select error.\n");

  return FD_ISSET(STDIN_FILENO, &rdfs);
}

char KbdCtrl::getChar() 
{
  extern int tty_cbreak(int fd);
  extern int tty_reset(int fd);
  char ch;
  int n;
  
  while (true) {
    tty_cbreak(0);
    if (isStdinReadable()) {
      if ((n = read(0, &ch, 1)) < 0)
        win->printw(name + ": read error.\n");
      else if (n == 0)
        win->printw(name + ": read 0 bytes.\n");
      else { // if (n > 0)
        tty_reset(0);
        break;
      }
    }
    tty_reset(0);
  }

  return ch;
}

//Msg key_msg(MSG_KEY, 's', NULL);

void KbdCtrl::handle(Msg *msg)
{
  //win->printw(name + ".handle() called.\n");
  key_msg.key = getChar();
  notifySubs(&key_msg);
}

void KbdCtrl::run() 
{
  //win->printw(name + ".run() begins.\n");
  Msg *msg;
  while (true) {
    mtx.lock();
    if (que.empty()) 
      msg = NULL;
    else {
      msg = que.front();
      que.pop();
    }
    mtx.unlock();
    //win->printw(name + ".run() wakes up.\n");

	  if (msg && msg->what == MSG_END)
	    break;

    handle(msg);
  }
  //win->printw(name + ".run() ends.\n");
}
