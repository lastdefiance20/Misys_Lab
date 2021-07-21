#include <iostream>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <termios.h>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <list>
#include <unistd.h>
#include <chrono>
#include <vector>
#include <pthread.h>

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include <locale.h>
#include <ncursesw/curses.h>
#include<map>

#include <CTetris.h>

using namespace std;

#define color_normal "\x1b[0m"
#define color_red "\x1b[31m"
#define color_green "\x1b[32m"
#define color_yellow "\x1b[33m"
#define color_blue "\x1b[34m"
#define color_magenta "\x1b[35m"
#define color_cyan "\x1b[36m"
#define color_white "\x1b[37m"
#define color_black "\x1b[30m"
#define color_pink "\x1b[95m"
#define b_color_black "\x1b[40m"

/**************************************************************/
/****************  Data model related code  *******************/
/**************************************************************/
#define MAX_BLK_TYPES 7
#define MAX_BLK_DEGREES 4

char saved_key = 0;
bool isGameDone = false;
int tty_cbreak(int fd);	/* put terminal into cbreak mode */
int tty_reset(int fd);	/* restore terminal's mode */
int ddy;
int ddx;

//T%d번으로 블럭의 타입을 설정하고, D%d번으로 오른쪽으로 90도씩 돌린 블럭의 state를 표현한다

//순서바꾸기

//T0 = I shape
int T0D0[] = { 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, -1 }; //I
int T0D1[] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, -1 };
int T0D2[] = { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1 };
int T0D3[] = { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, -1 };

//T1 = J shape
int T1D0[] = { 1, 0, 0, 1, 1, 1, 0, 0, 0, -1 };
int T1D1[] = { 0, 1, 1, 0, 1, 0, 0, 1, 0, -1 };
int T1D2[] = { 0, 0, 0, 1, 1, 1, 0, 0, 1, -1 };
int T1D3[] = { 0, 1, 0, 0, 1, 0, 1, 1, 0, -1 };

//T2 = L shape
int T2D0[] = { 0, 0, 1, 1, 1, 1, 0, 0, 0, -1 };
int T2D1[] = { 0, 1, 0, 0, 1, 0, 0, 1, 1, -1 };
int T2D2[] = { 0, 0, 0, 1, 1, 1, 1, 0, 0, -1 };
int T2D3[] = { 1, 1, 0, 0, 1, 0, 0, 1, 0, -1 };

//T3 = O shape
int T3D0[] = { 1, 1, 1, 1, -1 };
int T3D1[] = { 1, 1, 1, 1, -1 };
int T3D2[] = { 1, 1, 1, 1, -1 };
int T3D3[] = { 1, 1, 1, 1, -1 };

//T4 = S shape
int T4D0[] = { 0, 1, 1, 1, 1, 0, 0, 0, 0, -1 };
int T4D1[] = { 0, 1, 0, 0, 1, 1, 0, 0, 1, -1 };
int T4D2[] = { 0, 0, 0, 0, 1, 1, 1, 1, 0, -1 };
int T4D3[] = { 1, 0, 0, 1, 1, 0, 0, 1, 0, -1 };

//T5 = T shape
int T5D0[] = { 0, 1, 0, 1, 1, 1, 0, 0, 0, -1 };
int T5D1[] = { 0, 1, 0, 0, 1, 1, 0, 1, 0, -1 };
int T5D2[] = { 0, 0, 0, 1, 1, 1, 0, 1, 0, -1 };
int T5D3[] = { 0, 1, 0, 1, 1, 0, 0, 1, 0, -1 };

//T6 = Z shape
int T6D0[] = { 1, 1, 0, 0, 1, 1, 0, 0, 0, -1 };
int T6D1[] = { 0, 0, 1, 0, 1, 1, 0, 1, 0, -1 };
int T6D2[] = { 0, 0, 0, 1, 1, 0, 0, 1, 1, -1 };
int T6D3[] = { 0, 1, 0, 1, 1, 0, 1, 0, 0, -1 };

//블럭을 한번씩 계속 rotate시킨 청사진을 setOfCBlockArrays에 저장하여 블럭이 가능한 모든 state를 표현
//ex) ㅗ가 저장된 setOfBlockArrays = T1D0 ~ T1D3 = [ㅗ, ㅏ, ㅜ, ㅓ], 총 7가지 블럭
int *setOfCBlockArrays[] = {
  T0D0, T0D1, T0D2, T0D3,
  T1D0, T1D1, T1D2, T1D3,
  T2D0, T2D1, T2D2, T2D3,
  T3D0, T3D1, T3D2, T3D3,
  T4D0, T4D1, T4D2, T4D3,
  T5D0, T5D1, T5D2, T5D3,
  T6D0, T6D1, T6D2, T6D3,
};

/**************************************************************/
/****************         UI CODE         *********************/
/**************************************************************/
WINDOW * win0;
WINDOW * win1;
WINDOW * win2;
mutex* m;
condition_variable* cv;

/*
void printMsg(string msg){
  wclear(win0);

  mvwprintw(win0, 0, 0, msg);

  m->lock();
  wrefresh(win0);
  m->unlock();
  return;
}
*/

void printWindow(CTetris *board, int n){
  int dy = board->oCScreen.get_dy();
  int dx = board->oCScreen.get_dx();
  int dw = board->iScreenDw;
  int **array = board->oCScreen.get_array();
  
  if(n == 1){
    wclear(win1);

    for (int y = 0; y < dy - dw; y++) {
      for (int x = dw; x < dx - dw; x++) {
        if (array[y][x] == 0)
          mvwprintw(win1, y, x, "□");
        else if (array[y][x] < 8)
          mvwprintw(win1, y, x, "■");
        else 
          mvwprintw(win1, y, x, "X");
      }
      cout << endl;
    }
    wrefresh(win1);
  }
  else{
    wclear(win2);
    
    for (int y = 0; y < dy - dw; y++) {
      for (int x = dw; x < dx - dw; x++) {
        if (array[y][x] == 0)
          mvwprintw(win2, y, x, "□");
        else if (array[y][x] < 8)
          mvwprintw(win2, y, x, "■");
        else 
          mvwprintw(win2, y, x, "X");
      }
      cout << endl;
    }
    wrefresh(win2);
  }
}

char ggetch() {
  char ch;
  int n;
  while (1) {
    //charbreak - input에 char 하나가 들어오면 input을 종료(break)하고 들어온 char을 터미널에 저장?
    tty_cbreak(0);
    //read를 사용하여 저장된 char 가져옴?
    n = read(0, &ch, 1);
    //종료된 터미널을 다시 리셋
    tty_reset(0);
    if (n > 0)
      break;
    else if (n < 0) {
      if (errno == EINTR) {
        if (saved_key != 0) {
          ch = saved_key;
          saved_key = 0;
          break;
	      }
      }
    }
  }
  return ch;
}


/**************************************************************/
/*********      Threading code (Observer pattern)     *********/
/**************************************************************/

class View{
  public:
    queue<CTetris*> boards;
    int windownum;

    void update(CTetris* board){
      m->lock();
      boards.push(board);
      cv->notify_all();
      m->unlock();
    }

    CTetris* read(){
      unique_lock<mutex> lk(*m);
      //lock or not, need to wake up
      if(boards.empty()) cv->wait(lk, [&]{return !boards.empty();});
      CTetris* board = boards.front();
      boards.pop();
      lk.unlock();
      return board;
    }

    void addWindow(int n){
      windownum = n;
      return;
    }

    void run(){
      sleep(3);
      while(isGameDone == false) {
        CTetris *board = read();
        printWindow(board, windownum);
      }
    }
};

class Model : public View{
  public:
    list<View> observers;
    queue<char> keys;
    map<char, char> keypad;

    void addObserver(View observer){
      observers.push_back(observer);
      return;
    }

    void notifyObservers(CTetris* board){
      for(View observer : observers){
        observer.update(board);
      }
      return;
    }

    void update(char key){
      m->lock();
      keys.push(key);
      cv->notify_all();
      m->unlock();
      return;
    }

    char read(){
      unique_lock<mutex> lk(*m);
      //lock or not, need to wake up
      if(keys.empty()) cv->wait(lk, [&]{return !keys.empty();});
      char key = keys.front();
      keys.pop();
      lk.unlock();
      return key;
    }

    void addKeypad(map<char, char> keypads){
      keypad = keypads;
      return;
    }

    void run(){
      sleep(3);
      CTetris::init(setOfCBlockArrays, MAX_BLK_TYPES, MAX_BLK_DEGREES);
      CTetris *board = new CTetris(ddy, ddx);
      TetrisState state;
      char key;

      srand((unsigned int)time(NULL));
      key = (char)('0' + rand() % MAX_BLK_TYPES);
      state = board->accept(key);
      notifyObservers(board);

      while(isGameDone == false) {
        key = read();
        if(keypad.find(key)==keypad.end()){
          key = keypad[key];
          state = board->accept(key);
        }
        if (state == NewBlock) {
          key = (char)('0' + rand() % MAX_BLK_TYPES);
          state = board->accept(key);
          if (state == Finished) {
            notifyObservers(board);
            isGameDone = true;
            cout << endl;
            ;
            delete board;
            return;
          }
        }
        notifyObservers(board);
        cout << endl;
        if(key == 'q'){
          isGameDone = true;
          cout << endl;
          delete board;
          return;
        }
      }
      delete board;
    }
};


class KeyController : public Model{
  public:
    list<Model> observers;

    void addObserver(Model observer){
      observers.push_back(observer);
      return;
    }

    void notifyObservers(char key){
      for(Model observer : observers){
        observer.update(key);
      }
      return;
    }

    void run(){
      sleep(3);
      while(isGameDone == false){
        char key = ggetch();
        notifyObservers(key);

        if(key == 'q'){
          isGameDone = true;
          break;
        }
      }
    }
};

class TimeController : public Model{
  public:
    list<Model> observers;

    void addObserver(Model observer){
      observers.push_back(observer);
      return;
    }

    void notifyObservers(char key){
      for(Model observer : observers){
        observer.update(key);
      }
      return;
    }

    void run(){
      sleep(3);
      while(isGameDone == false){
        sleep(1);

        char key = 'y';
        notifyObservers(key);
      }
    }
};

/**************************************************************/
/****************        Main code        *********************/
/**************************************************************/


int main(int argc, char *argv[]){
  int dy, dx;
  char key = 0;

  if (argc != 3) {
    cout << "usage: " << argv[0] << " dy dx" << endl;
    exit(1);
  }
  if ((dy = atoi(argv[1])) <= 0 || (dx = atoi(argv[2])) <= 0) {
    cout << "dy and dx should be greater than 0" << endl;
    exit(1);
  }

  ddy = dy;
  ddx = dx;

  setlocale(LC_ALL, "");
  initscr();
  echo();
  start_color();
  refresh();

  win0 = newwin(3, 70, 21, 0);
  win1 = newwin(20, 30, 0, 0); 
  win2 = newwin(20, 30, 0, 40);

  View th_view1;
  th_view1.addWindow(1);

  View th_view2;
  th_view2.addWindow(2);

  map<char, char> keypad1 = {{'q', 'q'}, {'w', 'w'}, {'a', 'a'}, {'s', 's'}, {'d', 'd'}, {' ', ' '}, {'y', 's'}};
	Model th_model1;
	th_model1.addKeypad(keypad1);
	th_model1.addObserver(th_view1);

	map<char, char> keypad2 = {{'u', 'q'}, {'i', 'w'}, {'j', 'a'}, {'k', 's'}, {'l', 'd'}, {'\r', ' '}, {'y', 's'}};
	Model th_model2;
	th_model2.addKeypad(keypad2);
	th_model2.addObserver(th_view2);

	KeyController th_cont1;
	th_cont1.addObserver(th_model1);
	th_cont1.addObserver(th_model2);

  KeyController th_cont2;
	th_cont2.addObserver(th_model1);
	th_cont2.addObserver(th_model2);

/*
  vector<thread> threads;
  threads.push_back(thread([](){
    View th_view1;
    th_view1.run();
  }));
  threads.push_back(thread([](){
    View th_view2;
    th_view2.run();
  }));
  threads.push_back(thread([](){
	  Model th_model1;
    th_model1.run();
  }));
  threads.push_back(thread([](){
    Model th_model2;
    th_model2.run();
  }));
  threads.push_back(thread([](){
	  KeyController th_cont1;
    th_cont1.run();
  }));
  threads.push_back(thread([](){
    KeyController th_cont2;
    th_cont2.run();
  }));

  for (int i = 0; i < 1; i++) {
    threads[i].join();
  }
*/
}