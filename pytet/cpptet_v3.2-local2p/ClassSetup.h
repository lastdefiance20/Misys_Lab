#pragma once
#include <iostream>
#include <Matrix.h>
#include <CTetris.h>
#include <locale.h>
#include <ncursesw/curses.h>

class KeyObserver{
  public:
    virtual void update(char key){}
};

class KeyPublisher{
  public:
    virtual void addObserver(KeyObserver* observer){}
    virtual void notifyObservers(char key){}
};

class OutScreenObserver{
  public:
    virtual void update(CTetris* board){}
};

class OutScreenPublisher{
  public:
    virtual void addObserver(OutScreenObserver* observer){}
    virtual void notifyObservers(CTetris* board){}
};

class DelRectObserver{
  public:
    virtual void update(Matrix lines){}
};

class DelRectPublisher{
  public:
    virtual void addObserver(DelRectObserver* observer){}
    virtual void notifyObservers(Matrix lines){}
};

//void printMsg(string str);

void drawScreen(CTetris *board, WINDOW *win);