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

enum class GameState{Error=-1, Initial=0, Running=1, Waiting=2, Finished=3};
enum class GameCommand{NOP=-1, Quit=0, Start=1, Update=2, Accept=3};

extern GameState gamestate;

extern int stateTransMatrix[4][4];

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
        bool isServer; //해당 객체가 서버의 객체인지 클라이언트의 객체인지 구분하기 위한 변수
        KeyObserver* result;
        CTetris *board=new CTetris(20,15); 
        
        Model(string Name, bool ismine, bool isServer);
        void addKeypad(map<char,char>& keypad);

        void addObserverView(View* view);
        void notifyObserversView(Matrix* screen);
        
        void updateKey(char key);
        void addObserverKey(KeyObserver* send);
        void notifyObserversKey(char key);
        
        void acceptObserver(KeyObserver* Observer);

        void updateDel(Matrix delRect);
        void addObserverDel(Model* model);
        void notifyObserversDel(Matrix delRect);
        void executeCommand(GameCommand cmd, Obj obj);
        void startGame();
        void updateModel(Obj obj);
        void acceptResult();
        void quitGame();
        Obj Read();
        TetrisState processKey(CTetris* board, Obj obj);
        void printMsg(string msg);
        void run();
};
