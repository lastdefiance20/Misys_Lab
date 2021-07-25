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
#include <unistd.h>
#include <chrono>
#include <vector>

#include <locale.h>
#include <ncursesw/curses.h>
#include<map>

#include <CTetris.h>

using namespace std;

/**************************************************************/
/**************** Linux System Functions **********************/
/**************************************************************/

WINDOW * win0;
WINDOW * win1;
WINDOW * win2;

char saved_key = 0;
bool isGameDone = false;
bool returnName = false;
string loser;
int tty_cbreak(int fd);	/* put terminal into cbreak mode */
int tty_reset(int fd);	/* restore terminal's mode */
int ddy;
int ddx;

/* Read 1 character - echo defines echo mode */
// 키보드에서 char 읽음
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
/**************** Tetris Blocks Definitions *******************/
/**************************************************************/
#define MAX_BLK_TYPES 7
#define MAX_BLK_DEGREES 4

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

//drawScreen에서 refresh가 겹치면 window가 깨지는 현상 발생
//drawm 뮤택스로 동시에 window refresh 방지
mutex drawm;
void drawScreen(CTetris *board, WINDOW *win)
{
  int dy = board->oCScreen.get_dy();
  int dx = board->oCScreen.get_dx();
  int dw = board->iScreenDw;
  int **array = board->oCScreen.get_array();
  wclear(win);

  for (int y = 0; y < dy - dw; y++) {
    for (int x = dw; x < dx - dw; x++) {
      if (array[y][x] == 0){
        mvwaddstr(win, y, x, "□");
      }
      else if (array[y][x] < 8){
        //attron이 안되던 이유는 window 설정을 해주지 않아서임
        //앞에 w를 붙여 wattron으로 만들어서 색상 바꿀 창을 명시해줌
        wattron(win, COLOR_PAIR(array[y][x]));
        mvwaddstr(win, y, x, "■");
        wattroff(win, COLOR_PAIR(array[y][x]));
      }
      else 
        mvwaddstr(win, y, x, "X");
    }
    cout << endl;
  }
  drawm.lock();
  wrefresh(win);
  drawm.unlock();
}

//win0 창에 종료메세지 등 메세지를 출력한다
void printMsg(string str){
  //string -> char으로 변환하여 printw가 출력 가능한 char 형식으로 변환해준다
  const char *message = str.c_str();
  wclear(win0);
  wprintw(win0, message);
  //게임이 끝나고 출력해주기 때문에 refreash 겹칠 염려 X, 뮤택스 보호가 필요없다
  wrefresh(win0);
}

/**************************************************************/
/******************** Tetris Main Loop ************************/
/**************************************************************/

class Model{
  public:
    queue<char> keys;
    map<char, char> keypad;
    WINDOW *n;
    mutex m; //keys에 동시접근을 막기 위한 뮤택스
    condition_variable cv; //keys에 key가 들어옴을 알리기 위한 CV
    string pname;

    //플레이어 이름 설정
    Model(string name){
      pname = name;
    }

    void update(char key){
      m.lock();
      keys.push(key);
      cv.notify_all();
      m.unlock();
    }

    void setkeypad(map<char,char> keypad_ex){
      keypad = keypad_ex;
    }

    void setwindow(WINDOW *x){
      n = x;
    }

    char read(){
      unique_lock<mutex> lk(m);
      //keys에 key가 들어올때까지 대기, sleep 상태에서는 뮤택스 unlocked
      if(keys.empty()) cv.wait(lk, [&]{return !keys.empty();});
      char key = keys.front();
      keys.pop();
      lk.unlock();
      return key;
    }
  
    void run(){
      CTetris *board = new CTetris(ddy, ddx);
      TetrisState state;
      char key;

      srand((unsigned int)time(NULL));
      key = (char)('0' + rand() % MAX_BLK_TYPES);
      state = board->accept(key);
      drawScreen(board, n);

      while(isGameDone == false) {
        key = read();
        //keypad에 존재하면 keypad 딕셔너리 자료형으로 알맞는 key로
        //변환한 다음, key에 맞는 동작 수행
        if(keypad.find(key)!=keypad.end()){
          key = keypad[key];
          state = board->accept(key);

          if (state == NewBlock) {
            key = (char)('0' + rand() % MAX_BLK_TYPES);
            state = board->accept(key);
            if (state == Finished) {
              drawScreen(board, n);
              isGameDone = true;
              break;
            }
          }
          drawScreen(board, n);
          cout << endl;
          if(key == 'q'){
            isGameDone = true;
            break;
          }
        }
      }
      //진사람 출력(먼저 보드가 찬사람)
      if (returnName==false){
        returnName=true;
        loser=pname;
      }
      //delete board;
    }
};

class TimeController{
  public:
    Model* observers[2];
    int nobservers = 0;
    
    void addObserver(Model* observer){
      observers[nobservers] = observer;
      nobservers++;
    }

    void notifyObservers(char key){
      for(int i=0; i<nobservers; i++){
        observers[i]->update(key);
      }
    }

    void run(){
      //1초마다 아래로 내려가게 함
      while(isGameDone == false){
        sleep(1);
        notifyObservers('y');
      }
    }
};

class KeyController{
  public:
    Model* observers[2];
    int nobservers = 0;

    void addObserver(Model* observer){
      observers[nobservers] = observer;
      nobservers++;
    }

    void notifyObservers(char key){
      for(int i=0; i<nobservers; i++){
        observers[i]->update(key);
      }
    }

    void run(){
      //key값을 받아서 Main으로 전달해줌
      while(isGameDone == false){
        char key = ggetch();
        cout << key << endl;
        notifyObservers(key);
        if(key == 'q'){
          isGameDone = true;
          break;
        }
      }
    }
};

void ModelThread(Model* mclass){
  mclass->run();
}

void KeyControllerThread(KeyController* kclass){
  kclass->run();
}

void TimeControllerThread(TimeController* tclass){
  tclass->run();
}

int main(int argc, char *argv[]) {
  int dy, dx;
  char key = 0;

  //테트리스 창 크기 설정
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

  setlocale(LC_ALL, ""); //네모 출력을 위한 설정
  initscr(); //curses 라이브러리 창 시작
  start_color(); //color 사용 시작

  //color pair 생성, curses에서는 총 8가지 컬러 지원
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_BLACK);
  init_pair(4, COLOR_BLUE, COLOR_BLACK);
  init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(6, COLOR_CYAN, COLOR_BLACK);
  init_pair(7, COLOR_WHITE, COLOR_BLACK);

  //창 생성
  win0 = newwin(3, 70, 21, 0);
  win1 = newwin(20, 30, 0, 0); 
  win2 = newwin(20, 30, 0, 40);
  
  printMsg("테트리스 2인용입니다");

  //initialize는 한번으로 충분함
  CTetris::init(setOfCBlockArrays, MAX_BLK_TYPES, MAX_BLK_DEGREES);

  //1p, 2p 플레이어 키패드
  map<char,char> dic1={{'q','q'},{'w','w'},{'a','a'},{'s','s'},{'d','d'},{' ',' '},{'y','s'}};
  map<char, char> dic2 = {{'u', 'q'}, {'i', 'w'}, {'j', 'a'}, {'k', 's'}, {'l', 'd'}, {'\r', ' '}, {'y', 's'}};

  Model th_model1("player 1");
  th_model1.setkeypad(dic1);
  th_model1.setwindow(win1);

  Model th_model2("player 2");
  th_model2.setkeypad(dic2);
  th_model2.setwindow(win2);

  KeyController th_cont1;
	th_cont1.addObserver(&th_model1);
	th_cont1.addObserver(&th_model2);

  TimeController th_cont2;
	th_cont2.addObserver(&th_model1);
	th_cont2.addObserver(&th_model2);

  vector<thread> threads;
  threads.push_back(thread(ModelThread, &th_model1));
  threads.push_back(thread(ModelThread, &th_model2));
  threads.push_back(thread(KeyControllerThread, &th_cont1));
  threads.push_back(thread(TimeControllerThread, &th_cont2));

  //0번 메인 쓰레드의 종료만 확인한다. 이 친구가 종료되면
  //다른 친구들이 종료될때 까지 기다려줄 필요가 없기 때문이다.
  //(isGameDone == false임이 자명하기 때문에)
  threads[0].join();

  //진사람 출력(먼저 보드가 찬사람)
  string endMsg = "Program terminated! ";
  endMsg.append(loser);
  endMsg.append(" Lose");
  printMsg(endMsg);
  
  //delete board를 이용하여CTetris의 소멸자가 두번 발동되면 지운 주소에
  //다시 접근하여 에러가 발생함. 따라서 한번만 소멸하도록 수동으로 수행
  CTetris::deleteonce();
  sleep(5);
  endwin();
  exit(0);
  //return 0;
}