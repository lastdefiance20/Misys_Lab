#pragma once
#include "Interface.h"
#include <ncurses.h>
#include <map>
#include <mutex>
#include <condition_variable>
#include <unistd.h>

#define MAX_BLK_TYPES 7
#define MAX_BLK_DEGREES 4

extern bool isGameDone;
extern std::mutex mu;

class View:public OutScreenObserver{
    public:
        string name;
        std::queue<Matrix*> Screens;
        WINDOW* win;
        std::condition_variable cv; 
        std::mutex m;

        View(string Name);
        void addWindow(WINDOW* window); 

        void updateView(Matrix* screen);
       
        Matrix* read();
        void printWindow(WINDOW *window, Matrix& screen);
        void run();
};

class Model:public KeyObserver,public OutScreenPublisher,public delRectObserver,public delRectPublisher{ //Model 클래스
    public:
        string name;
        std::vector<View*> observers_view; 
        std::vector<KeyObserver*> observers_send;
        std::vector<Model*> models;
        std::queue<Obj> objs; 
        std::map<char,char> Keypad;
        std::condition_variable cv;
        std::mutex m; 
        bool isMine;
        bool isServer;
        
        Model(string Name, bool ismine, bool isServer);
        void addKeypad(map<char,char>& keypad);

        void addObserverView(View* view);
        void notifyObserversView(Matrix* screen);
        
        void updateKey(char key);
        void addObserverKey(KeyObserver* send);
        void notifyObserversKey(char key);
        
        void updateDel(Matrix delRect);
        void addObserverDel(Model* model);
        void notifyObserversDel(Matrix delRect);

        Obj read();
        TetrisState processKey(CTetris* board, Obj obj);
        void run();
};