#pragma once
#include <iostream>
#include <Matrix.h>
#include <CTetris.h>
#include <locale.h>
#include <ncursesw/curses.h>

extern string endmsg;
extern string statemsg;

enum ClientState{
  cError = -1,
  cInitial = 0,
  cRunning = 1,
  cReceiving = 2,
  cResponding = 3,
  cFinished = 4
};

enum ServerState{
  sError = -1,
  sWaiting = 0,
  sRunning = 1,
  sTerminatingP = 2,
  sTerminatingS = 3,
  //sCheckingP = 4,
  //sCheckingallP = 5
};

enum ClientCommand{
  cStart = 0,
  cFinish = 1,
  cReceive = 2,
  cUpdate = 3
};

enum ServerCommand{
  sStart = 0,
  sFinishQ = 1,
  sFinishM = 2,
  sReceive = 3,
  sUpdate = 4
};

extern ServerState serverState;
extern ClientState clientState;
extern WINDOW * win3;

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

void printMsg(string str, WINDOW *win);

void printStateMsg(string str, WINDOW *win);

void drawClientState();

void drawClientState(ClientCommand cc);

void drawServerState();

void drawServerState(ServerCommand sc);