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

#include <ncurses.h>

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
condition_variable cv;

void printMsg(string msg){
  wclear(win0);

  mvwprintw(win0, 0, 0, msg)

  m->lock();
  wrefresh(win0);
  m->unlock();
  return;
}

string arrayToString(int array[]){
  string line;

  for(int i=0; i<15; i++){
    int x = array[i];
    if(x==0) line.push_back('□');
    else if(x == 1) line.push_back('■');
    else line.push_back('XX');
  }

  return line;
}


void printWindow(window, screen){
 int** array = screen.get_array()
 wclear(window)

 for(int y=0; y<(screen.get_dy()-Tetris.iScreenDw); y++){
  string line = arrayToString(array[y][Tetris.iScreenDw:-Tetris.iScreenDw])
  mvwprintw(window, y, 0, line);
 }
 
 m->lock();
 wrefresh(window);
 m->unlock();
 return;
}


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


/**************************************************************/
/*********      Threading code (Observer pattern)     *********/
/**************************************************************/


class Keycontroller{
  public:
    void add

}



/**************************************************************/
/****************        Main code        *********************/
/**************************************************************/


int main(){
  //queue<char> keys;
  mutex lock;
  //condition_variable cv;

  screen = initstr();
  screen.clear();

  win1 = newwin(20, 30, 0, 0); 
  win2 = newwin(20, 30, 0, 40);
  win0 = newwin(3, 70, 21, 0);

  th_view1 = View('view1');
  th_view1.addWindow(win1);

  th_view2 = View('view2');
  th_view2.addWindow(win2);

  keypad1 = { 'q': 'q', 'w': 'w', 'a': 'a', 's': 'y', 'd': 'd', ' ': ' ', 'y': 'y' }
	th_model1 = Model('model1')
	th_model1.addKeypad(keypad1)
	th_model1.addObserver(th_view1)

	keypad2 = { 'u': 'q', 'i': 'w', 'j': 'a', 'k': 'y', 'l': 'd', '\r': ' ', 'y': 'y' }
	th_model2 = Model('model2')
	th_model2.addKeypad(keypad2)
	th_model2.addObserver(th_view2)

	th_cont1 = KeyController('kcont')
	th_cont1.addObserver(th_model1)
	th_cont1.addObserver(th_model2)

	th_cont2 = TimeController('tcont')
	th_cont2.addObserver(th_model1)
	th_cont2.addObserver(th_model2)

  vector<thread> threads;
  threads.push_back(thread(th_view1, &lock));
  threads.push_back(thread(th_view2, &lock));
  threads.push_back(thread(th_mode11, &lock));
  threads.push_back(thread(th_mode12, &lock));
  threads.push_back(thread(th_cont1, &lock));
  threads.push_back(thread(th_cont2, &lock));

  for (int i = 0; i < 3; i++) {
    threads[i].join();
  }
}