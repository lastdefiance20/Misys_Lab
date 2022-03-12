#pragma once

#include <mutex>
#include <ncursesw/curses.h>

using namespace std;

class Window {
 private:
  static mutex mtx;
  WINDOW *win;
  int nrows;
  int ncols;
  int curr_row;
  
 public:
  Window(WINDOW *w);
  void clear();
  void addStr(int y, int x, const char *s); // add a plain string
  void addCstr(int y, int x, const char *s, int color); // add a colored string
  void printw(string s);
  void refresh();
};

