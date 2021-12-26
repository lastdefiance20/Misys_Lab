#pragma once
#include <ClassSetup.h>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <string>

extern bool isGameDone;

class View: public OutScreenObserver{
  public:
    queue<CTetris*> boards;
    WINDOW *n;
    mutex m; //keys에 동시접근을 막기 위한 뮤택스
    condition_variable cv; //keys에 key가 들어옴을 알리기 위한 CV
    string vname;
    WINDOW *prwin;

    View(string name, WINDOW *win);
    virtual void update(CTetris* board);
    void setwindow(WINDOW *x);
    void run();
};