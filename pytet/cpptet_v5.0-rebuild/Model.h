#pragma once
#include <mutex>
#include <queue>
#include <condition_variable>
#include <string>
#include <map>
#include <list>
#include <ncursesw/curses.h>
#include <OutScreenObserver.h>
#include <CTetris.h>

extern bool isGameDone;

class View;

class Model: public KeyObserver, OutScreenPublisher{
  public:
    list<Matrix> MatrixKeys;
    mutex m; //keys에 동시접근을 막기 위한 뮤택스
    condition_variable cv; //keys에 key가 들어옴을 알리기 위한 CV
    string pname;
    View* observers[2];
    int nobservers = 0;
    int Mnobservers = 0;

    virtual void addObserver(View* observer);
    virtual void notifyObservers(Matrix board);
    Model(string name);
    virtual void update(Matrix key);
    Matrix read();
    Matrix makeScreen(CTetris* board);
    void run();
};