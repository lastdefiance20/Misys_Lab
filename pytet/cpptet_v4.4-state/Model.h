#pragma once
#include <ClassSetup.h>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <string>
#include <map>
#include <list>

extern bool isGameDone;
extern bool returnName;
extern mutex sendmutex;
class View;
class SendController;

class Model: public KeyPublisher, KeyObserver, OutScreenPublisher, 
DelRectObserver, DelRectPublisher{
  public:
    list<keyBox> CBs;
    map<char, char> keypad;
    mutex m; //keys에 동시접근을 막기 위한 뮤택스
    condition_variable cv; //keys에 key가 들어옴을 알리기 위한 CV
    string pname;
    string winlose;
    View* observers[2];
    Model* Mobservers[2];
    SendController* Sobservers[2];
    int nobservers = 0;
    int Mnobservers = 0;
    int Snobservers = 0;

    bool isMainModel = false;
    bool isReferee = false;

    Matrix tmpdelRect;
    Matrix delRect;

    WINDOW *prwin;

    virtual void addObserver(View* observer);

    virtual void addObserver(Model* Mobserver);
    
    virtual void addObserver(SendController* Sobserver);

    virtual void notifyObservers(Matrix lines);

    virtual void notifyObservers(CTetris* board);
    
    virtual void notifyObservers(char key);

    Model(string name, WINDOW *win);

    //key를 update할 경우
    virtual void update(char key);

    //지운 line을 update할 경우
    virtual void update(Matrix lines);

    void setkeypad(map<char,char> keypad_ex);

    void setmainmodel();

    void setrefree();

    keyBox read();

    keyBox check();

    keyBox swap();
  
    void terminating();//s
    void receivingM();
    void receivingQ();
    void responding();
    TetrisState running(CTetris *board, TetrisState state);

    void run();
};