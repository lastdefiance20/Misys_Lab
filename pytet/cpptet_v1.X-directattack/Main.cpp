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
#include <list>

#include <locale.h>
#include <ncursesw/curses.h>
#include <map>

#include <sys/types.h>
#include <sys/time.h>

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

int kbhit (void)
{
  struct timeval tv; //timeval에는 sec와 usec를 가지고 있다
  fd_set rdfs; //파일 디스크립터, 입력/출력 리소스에 액세스하는 데 사용되는 추상표현
 
  tv.tv_sec = 1;
  tv.tv_usec = 0;
 
  FD_ZERO(&rdfs); //fds 스트럭쳐를 0으로 초기화

  //FD가 STDIN_FILENO(0)에 해당하는 파일(표준입력파일)
  //에서 데이터가 변하는지(이벤트가 발생하는지) 체크할거니까 1로 세팅
  FD_SET (STDIN_FILENO, &rdfs);
 
  //tv는 함수 호출 후, 무한 대기 상태에 빠지지 않도록 타임-아웃(time-out) 설정
  //select 이후 변화가생긴 파일 디스크립터는 1로 세팅됨
  select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);

  //fds 스트럭처에 있는 stdin이 1로 세팅되어 있는가?
  return FD_ISSET(STDIN_FILENO, &rdfs); //==이벤트가 발생 했는가

  //https://moolgogiheart.tistory.com/72
  //https://orange-makiyato.tistory.com/6
  //https://yms2047.tistory.com/entry/select-함수-사용법
}

/* Read 1 character - echo defines echo mode */
// 키보드에서 char 읽음
char ggetch() {
  char ch;
  int n;
  while (1) {
    tty_cbreak(0);
    if(kbhit()){
      //charbreak - input에 char 하나가 들어오면 input을 종료(break)하고 들어온 char을 터미널에 저장?
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
    //1초마다 확인한다
    if(isGameDone == true){
      ch = 'q';
      break;
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

string endmsg;
//win0 창에 종료메세지 등 메세지를 출력한다
void printMsg(string str){
  endmsg.append(str);
  //string -> char으로 변환하여 printw가 출력 가능한 char 형식으로 변환해준다
  const char *message = endmsg.c_str();
  wclear(win0);
  wprintw(win0, message);
  drawm.lock();
  wrefresh(win0);
  drawm.unlock();
}

/**************************************************************/
/******************** Tetris Main Loop ************************/
/**************************************************************/

//상속받을 class
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

class View: public OutScreenObserver{
  public:
    queue<CTetris*> boards;
    WINDOW *n;
    mutex m; //keys에 동시접근을 막기 위한 뮤택스
    condition_variable cv; //keys에 key가 들어옴을 알리기 위한 CV
    string vname;

    View(string name){
      vname = name;
    }

    virtual void update(CTetris* board){
      m.lock();
      boards.push(board);
      cv.notify_all();
      m.unlock();
      return;
    }

    void setwindow(WINDOW *x){
      n = x;
    }

    void run(){
      while(isGameDone == false){
        unique_lock<mutex> lk(m);
        if(boards.empty()) cv.wait(lk, [&]{return !boards.empty();});
        CTetris* board = boards.front();
        boards.pop();
        lk.unlock();
        drawScreen(board, n);
      }
      string breaked = " breaked\n";
      vname.append(breaked);
      printMsg(vname);
    }
};

class Model: public KeyObserver, OutScreenPublisher, 
DelRectObserver, DelRectPublisher{
  public:
    list<keyBox> CBs;
    map<char, char> keypad;
    mutex m; //keys에 동시접근을 막기 위한 뮤택스
    condition_variable cv; //keys에 key가 들어옴을 알리기 위한 CV
    string pname;
    View* observers[2];
    Model* Mobservers[2];
    int nobservers = 0;
    int Mnobservers = 0;

    Matrix tmpdelRect;
    Matrix delRect;

    virtual void addObserver(View* observer){
      observers[nobservers] = observer;
      nobservers++;
    }

    virtual void addObserver(Model* Mobserver){
      Mobservers[Mnobservers] = Mobserver;
      Mnobservers++;
    }

    virtual void notifyObservers(Matrix lines){
      for(int i=0; i<Mnobservers; i++){
        Mobservers[i]->update(lines);
      }
    }

    virtual void notifyObservers(CTetris* board){
      for(int i=0; i<nobservers; i++){
        observers[i]->update(board);
      }
    }

    Model(string name){
      pname = name;
    }

    //key를 update할 경우
    virtual void update(char key){
      struct keyBox CB;
      CB.key = key;
      CB.iskey = true;
      m.lock();
      CBs.push_back(CB);
      cv.notify_all();
      m.unlock();
    }

    //지운 line을 update할 경우
    virtual void update(Matrix lines){
      struct keyBox CB;
      CB.lines = lines;
      CB.iskey = false;
      m.lock();
      CBs.push_front(CB);
      cv.notify_all();
      m.unlock();
    }

    void setkeypad(map<char,char> keypad_ex){
      keypad = keypad_ex;
    }

    keyBox read(){
      unique_lock<mutex> lk(m);
      //keys에 key가 들어올때까지 대기, sleep 상태에서는 뮤택스 unlocked
      if(CBs.empty()) cv.wait(lk, [&]{return !CBs.empty();});
      struct keyBox CB = CBs.front();
      CBs.pop_front();
      lk.unlock();
      return CB;
    }
  
    void run(){
      CTetris *board = new CTetris(ddy, ddx);
      TetrisState state;
      char key;
      struct keyBox CB;

      srand((unsigned int)time(NULL));
      key = (char)('0' + rand() % MAX_BLK_TYPES);
      CB.iskey = true;
      CB.key = key;
      state = board->accept(CB);
      notifyObservers(board);
      
      while(isGameDone == false) {
        //NewBlockDelR = 줄을 지워서 temp에 저장했을때
        struct keyBox CB = read();
        if(CB.iskey==true){
          key = CB.key;
          //keypad에 존재하면 딕셔너리를 이용해 알맞는 key로 변환
          if(keypad.find(key)!=keypad.end()){
            CB.key = keypad[key];
            state = board->accept(CB);
            if (state == NewBlock) {
              CB.key = (char)('0' + rand() % MAX_BLK_TYPES);
              state = board->accept(CB);
              if (state == Finished) {
                notifyObservers(board);
                isGameDone = true;
                break;
              }
            }
            if (state == NewBlockDelR) {
              //printMsg("timetodel");
              delRect = board->getDelRect();
              notifyObservers(delRect);
              state = Running;
            }
            notifyObservers(board);
            cout << endl;
            if(key == 'q'){
              isGameDone = true;
              break;
            }
          }
        }
        else{
          state = board->accept(CB);
          notifyObservers(board);
          //printMsg("getdel");
        }
      }
      //view가 빠져나올수 있도록 board를 전달해준다
      notifyObservers(board);
      //진사람 출력(먼저 보드가 찬사람)
      if (returnName==false){
        returnName=true;
        loser=pname;
      }
      string breaked = " breaked\n";
      pname.append(breaked);
      printMsg(pname);
      //cout<<board->currCBlk<<endl;
      delete board;
      //cout<<"deleted really"<<endl;
    }
};

class TimeController: public KeyPublisher{
  public:
    Model* observers[2];
    int nobservers = 0;
    string cname;

    //컨트롤러 이름 설정
    TimeController(string name){
      cname = name;
    }

    virtual void addObserver(Model* observer){
      observers[nobservers] = observer;
      nobservers++;
    }

    virtual void notifyObservers(char key){
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
      string breaked = " breaked\n";
      cname.append(breaked);
      printMsg(cname);
    }
};

//키컨트롤러가 중간에 빠져나가는 방법이 필요하다
class KeyController: public KeyPublisher{
  public:
    Model* observers[2];
    int nobservers = 0;
    string cname;

    //컨트롤러 이름 설정
    KeyController(string name){
      cname = name;
    }

    virtual void addObserver(Model* observer){
      observers[nobservers] = observer;
      nobservers++;
    }

    virtual void notifyObservers(char key){
      for(int i=0; i<nobservers; i++){
        observers[i]->update(key);
      }
    }

    void run(){
      //key값을 받아서 Main으로 전달해줌
      while(isGameDone == false){
        char key = ggetch();
        notifyObservers(key);
        if(key == 'q'){
          isGameDone = true;
          break;
        }
      }
      string breaked = " breaked\n";
      cname.append(breaked);
      printMsg(cname);
    }
};

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
  win0 = newwin(8, 70, 21, 0);
  win1 = newwin(20, 30, 0, 0); 
  win2 = newwin(20, 30, 0, 40);
  
  printMsg("테트리스 2인용입니다\n");

  //initialize는 한번으로 충분함
  CTetris::init(setOfCBlockArrays, MAX_BLK_TYPES, MAX_BLK_DEGREES);

  //1p, 2p 플레이어 키패드
  map<char,char> dic1={{'q','q'},{'w','w'},{'a','a'},{'s','s'},{'d','d'},{' ',' '},{'y','s'}};
  map<char, char> dic2 = {{'u', 'q'}, {'i', 'w'}, {'j', 'a'}, {'k', 's'}, {'l', 'd'}, {'\r', ' '}, {'y', 's'}};

  View th_view1("Viewer 1");
  th_view1.setwindow(win1);

  View th_view2("Viewer 2");
  th_view2.setwindow(win2);

  Model th_model1("player 1");
  th_model1.setkeypad(dic1);
  th_model1.addObserver(&th_view1);

  Model th_model2("player 2");
  th_model2.setkeypad(dic2);
  th_model2.addObserver(&th_view2);

  th_model1.addObserver(&th_model2);
  th_model2.addObserver(&th_model1);

  KeyController th_cont1("keycontrol");
	th_cont1.addObserver(&th_model1);
	th_cont1.addObserver(&th_model2);

  TimeController th_cont2("timecontrol");
	th_cont2.addObserver(&th_model1);
	th_cont2.addObserver(&th_model2);

  vector<thread> threads;
  threads.push_back(thread(&View::run, &th_view1));
  threads.push_back(thread(&View::run, &th_view2));
  threads.push_back(thread(&Model::run, &th_model1));
  threads.push_back(thread(&Model::run, &th_model2));
  threads.push_back(thread(&KeyController::run, &th_cont1));
  threads.push_back(thread(&TimeController::run, &th_cont2));
  //https://stackoverflow.com/questions/10998780/stdthread-calling-method-of-class
  //이런 방식을 사용하면 따로 함수를 정의하지 않고 쓰레드로 넣을 수 있다
  //원하는 클래스의 멤버 함수와 클래스를 넣어주면 된다

  for(int i = 0; i < 6; i++){
    threads[i].join();
  }

  //진사람 출력(먼저 보드가 찬사람)
  string endMsg = "Program terminated! ";
  endMsg.append(loser);
  endMsg.append(" Lose");
  printMsg(endMsg);

  sleep(5);
  endwin();
  //static 변수는 같은 주소 공유, 한번만 삭제 or 삭제X!
}