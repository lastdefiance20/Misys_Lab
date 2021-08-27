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
/**************** Linux System Functions **********************/
/**************************************************************/

char saved_key = 0;
bool isGameDone = false;
int tty_cbreak(int fd);	/* put terminal into cbreak mode */
int tty_reset(int fd);	/* restore terminal's mode */
int ddy;
int ddx;

/* Read 1 character - echo defines echo mode */
// 키보드에서 char 읽음
char getch() {
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

void sigint_handler(int signo) {
  // cout << "SIGINT received!" << endl;
  // do nothing;
}

void sigalrm_handler(int signo) {
  alarm(1);
  saved_key = 's';
}

void unregisterAlarm() {
	alarm(0);
}

void registerAlarm() {
  struct sigaction act, oact;
  act.sa_handler = sigalrm_handler;
  sigemptyset(&act.sa_mask);
#ifdef SA_INTERRUPT
  act.sa_flags = SA_INTERRUPT;
#else
  act.sa_flags = 0;
#endif
  if (sigaction(SIGALRM, &act, &oact) < 0) {
    cerr << "sigaction error" << endl;
    exit(1);
  }
  alarm(1);
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


//void drawScreen(CTetris *board)
//void drawScreen(Tetris *board)
void drawScreen(CTetris *board)
{
  int dy = board->oCScreen.get_dy();
  int dx = board->oCScreen.get_dx();
  int dw = board->iScreenDw;
  int **array = board->oCScreen.get_array();
  system("clear");

  for (int y = 0; y < dy - dw; y++) {
    for (int x = dw; x < dx - dw; x++) {
      if (array[y][x] == 0)
        cout << color_white << "□" << color_normal;
      else if (array[y][x] == 1)
        cout << color_red<< "■" << color_normal;
      else if (array[y][x] == 2)
        cout << color_green << "■" << color_normal;
      else if (array[y][x] == 3)
        cout << color_yellow << "■" << color_normal;
      else if (array[y][x] == 4)
        cout << color_blue << "■" << color_normal;
      else if (array[y][x] == 5)
        cout << color_magenta << "■" << color_normal;
      else if (array[y][x] == 6)
        cout << color_cyan << "■" << color_normal;
      else if (array[y][x] == 7)
        cout << color_pink << "■" << color_normal;
      else 
        cout << b_color_black << "XX" << color_normal;
    }
    cout << endl;
  }
}

/**************************************************************/
/******************** Tetris Main Loop ************************/
/**************************************************************/

void TimeOutProducer(queue<char>* keys, mutex* m, condition_variable* cv){
  while(isGameDone == false){
    sleep(1);
    
    // data 는 쓰레드 사이에서 공유되므로 critical section 에 넣어야 한다.
    m->lock();
    keys->push('s');
    cv->notify_one();
    m->unlock();

    // consumer 에게 content 가 준비되었음을 알린다 (wake up consumer!).
    //cv->notify_one();
  }
}

void keyproducer(queue<char>* keys, mutex* m, condition_variable* cv){
  while(isGameDone == false){
    char key = getch();
    cout << key << endl;

    // data 는 쓰레드 사이에서 공유되므로 critical section 에 넣어야 한다.
    m->lock();
    keys->push(key);
    cv->notify_one();
    m->unlock();

    // consumer 에게 content 가 준비되었음을 알린다 (wake up consumer!).
    //cv->notify_one();

    if(key == 'q'){
      isGameDone = true;
			break;
    }
  }
}

void Consumer(queue<char>* keys, mutex* m, condition_variable* cv){

  CTetris::init(setOfCBlockArrays, MAX_BLK_TYPES, MAX_BLK_DEGREES);
  CTetris *board = new CTetris(ddy, ddx);
  TetrisState state;
  char key;

  //흑백버전 실행
  /*
  cout<<"11"<<endl;
  Tetris::init(setOfCBlockArrays, MAX_BLK_TYPES, MAX_BLK_DEGREES);
  cout<<"22"<<endl;
  Tetris *board = new Tetris(dy, dx);
  cout<<"33"<<endl;
  TetrisState state;
  cout<<"44"<<endl;
  */

  srand((unsigned int)time(NULL));
  key = (char)('0' + rand() % MAX_BLK_TYPES);
  state = board->accept(key);
  drawScreen(board);
  cout << endl;

  while(isGameDone == false) {
    unique_lock<mutex> lk(*m);

    //lock or not, need to wake up
    if(keys->empty()) cv->wait(lk, [&]{return !keys->empty();});

    key = keys->front();
    keys->pop();
    //unlock
    lk.unlock();

    state = board->accept(key);
    if (state == NewBlock) {
      key = (char)('0' + rand() % MAX_BLK_TYPES);
      state = board->accept(key);
      if (state == Finished) {
        drawScreen(board);
        isGameDone = true;
        cout << endl;
        ;
        delete board;
        return;
      }
    }
    drawScreen(board);
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

/*
$ make
$ ./Main.exe 20 10          # 세로 20 x 가로 10 의 배경화면을 생성하라는 의미임
*/

int main(int argc, char *argv[]) {
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

  queue<char> keys;
  mutex m;
  condition_variable cv;

  vector<thread> threads;
  threads.push_back(thread(Consumer, &keys, &m, &cv));
  threads.push_back(thread(keyproducer, &keys, &m, &cv));
  threads.push_back(thread(TimeOutProducer, &keys, &m, &cv));

  threads[0].join();
  /*
  for (int i = 0; i < 3; i++) {
    threads[i].join();
  }
  */
  cout << "Program terminated!" << endl;
  exit(0);
  //return 0;
}