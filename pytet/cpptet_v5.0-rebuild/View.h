#pragma once
#include <mutex>
#include <queue>
#include <condition_variable>
#include <string>
#include <ncursesw/curses.h>
#include <OutScreenObserver.h>
#include <CTetris.h>

extern bool isGameDone;

class View: public OutScreenObserver{
  public:
    queue<Matrix> boards; //matrix를 저장할 queue
    WINDOW *drawwin; //그려질 window
    WINDOW *prwin; //console print될 window
    mutex m; //keys에 동시접근을 막기 위한 뮤택스
    mutex drawm; //동시접근 draw를 막기 위한 뮤택스
    condition_variable cv; //keys에 key가 들어옴을 알리기 위한 CV
    string vname; //view 객체의 이름

    View(string name, WINDOW *win);
    virtual void update(Matrix board);
    void setwindow(WINDOW *x);
    void drawScreen(Matrix board, WINDOW *win);
    void run();
};