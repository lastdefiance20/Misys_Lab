#include "Window.h"

Window::Window(WINDOW *w) 
{ 
  win = w; 
  getmaxyx(w, nrows, ncols);
  curr_row = 0; 
  }

void Window::clear() 
{
  wclear(win);
}

void Window::addStr(int y, int x, const char *s) 
{
  mvwaddstr(win, y, x, s);
}

void Window::addCstr(int y, int x, const char *s, int color) 
{
  wattron(win, COLOR_PAIR(color));
  mvwaddstr(win, y, x, s);
  wattroff(win, COLOR_PAIR(color));
}

void Window::refresh() 
{
  mtx.lock();
  wrefresh(win);
  mtx.unlock();
}

void Window::printw(string s) 
{
  if (curr_row >= nrows) {
    curr_row = 0;
    clear();
  }

  int count = 0;
  for (int i = 0; i < s.size(); i++)
    if (s[i] == '\n') count++;

  curr_row += count;

  wprintw(win, s.c_str());
  refresh();
}

