#include <unistd.h>
#include <string>
#include <thread>
#include <vector>

#include "Window.h"
#include "View.h"
#include "Model.h"
#include "TimeCtrl.h"
#include "KbdCtrl.h"

#include "CTetris.h"
#include "ThreadCtrl.h"
#include "RandomCtrl.h"
#include "ReplayCtrl.h"
#include "LogCtrl.h"

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
/*
int T0D0[] = { 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, -1 };
int T0D1[] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, -1 };
int T0D2[] = { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1 };
int T0D3[] = { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, -1 };
*/
Matrix m0(T0D0, 4, 4);
Matrix m1(T0D1, 4, 4);
Matrix m2(T0D2, 4, 4);
Matrix m3(T0D3, 4, 4);
  
Msg msg0(MSG_MAT, 0, &m0);
Msg msg1(MSG_MAT, 0, &m1);
Msg msg2(MSG_MAT, 0, &m2);
Msg msg3(MSG_MAT, 0, &m3);

Msg msg_end(MSG_END, 0, NULL);

void init_screen() 
{
  setlocale(LC_ALL, ""); // for printing a box character
  initscr();         // initialize the curses screen
  start_color(); // start using colors
  // init_pair(index, fg color, bg color);
  init_pair(1, COLOR_RED,     COLOR_BLACK); 
  init_pair(2, COLOR_GREEN,   COLOR_BLACK);
  init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
  init_pair(4, COLOR_BLUE,    COLOR_BLACK);
  init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(6, COLOR_CYAN,    COLOR_BLACK);
  init_pair(7, COLOR_WHITE,   COLOR_BLACK);
}

void close_screen() 
{
  endwin();
}

int main(int argc, char *argv[]) 
{
  int num;

  cout << "Type 1 or 2" << endl;
  cout << "1 => Play Tetris Game" << endl;
  cout << "2 => Replay Recent Tetris Game" << endl;
  cin >> num;

  vector<Sub*> sub_list;
  vector<thread*> task_list;
  thread *task;

  //init screen
  init_screen();

  //init ctetris
  CTetris::init(setOfCBlockArrays, MAX_BLK_TYPES, MAX_BLK_DEGREES);

  // newwin(nlines, ncolumns, begin_y, begin_x)
  Window bttm_win(newwin(8, 50, 21, 0));
  Window left_win(newwin(20, 30, 0, 0));
  Window rght_win(newwin(20, 30, 0, 40));
  //bttm_win.printw("This is the bottom window.\n");

  // create tasks to compose a graph
  if(num == 1){
    View *left_view = new View(&left_win, &bttm_win, "left_view");
    Model *left_model = new Model(&bttm_win, "left_model");
    TimeCtrl *time_ctrl = new TimeCtrl(&bttm_win, "time_ctrl", 1);
    KbdCtrl *kbd_ctrl = new KbdCtrl(&bttm_win, "kbd_ctrl");
    RandomCtrl *random_ctrl = new RandomCtrl(&bttm_win, "random_ctrl");
    LogCtrl *log_ctrl = new LogCtrl(&bttm_win, "log_ctrl");

    //control thread
    ThreadCtrl *thread_ctrl = new ThreadCtrl(&bttm_win, "thread_ctrl");
    sub_list.push_back(thread_ctrl);
    thread_ctrl->addSubs(left_model);
    thread_ctrl->addSubs(time_ctrl);
    thread_ctrl->addSubs(kbd_ctrl);
    thread_ctrl->addSubs(left_view);
    thread_ctrl->addSubs(thread_ctrl);
    thread_ctrl->addSubs(random_ctrl);
    thread_ctrl->addSubs(log_ctrl);

    // connect tasks to compose the graph
    left_model->addSubs(left_view);
    time_ctrl->addSubs(left_model);
    kbd_ctrl->addSubs(left_model);

    left_model->addSubs(thread_ctrl); //add
    left_model->addSubs(random_ctrl);

    random_ctrl->addSubs(left_model);

    time_ctrl->addSubs(log_ctrl);
    kbd_ctrl->addSubs(log_ctrl);
    random_ctrl->addSubs(log_ctrl);

    // run a thread for each task
    task = new thread(&View::run, left_view);
    task_list.push_back(task);
    task = new thread(&Model::run, left_model);
    task_list.push_back(task);
    task = new thread(&TimeCtrl::run, time_ctrl);
    task_list.push_back(task);
    task = new thread(&KbdCtrl::run, kbd_ctrl);
    task_list.push_back(task);
    task = new thread(&ThreadCtrl::run, thread_ctrl);
    task_list.push_back(task); //add
    task = new thread(&RandomCtrl::run, random_ctrl);
    task_list.push_back(task); //add
    task = new thread(&LogCtrl::run, log_ctrl);
    task_list.push_back(task); //add

    // wait for each task to be terminated
    for (int i=0; i < task_list.size(); i++) 
      task_list[i]->join(); 

    close_screen();
  }
  else if(num == 2){
    View *left_view = new View(&left_win, &bttm_win, "left_view");
    Model *left_model = new Model(&bttm_win, "left_model");
    TimeCtrl *time_ctrl = new TimeCtrl(&bttm_win, "time_ctrl", 0.2);
    ReplayCtrl *replay_ctrl = new ReplayCtrl(&bttm_win, "replay_ctrl");

    //control thread
    ThreadCtrl *thread_ctrl = new ThreadCtrl(&bttm_win, "thread_ctrl");
    thread_ctrl->addSubs(left_model);
    thread_ctrl->addSubs(time_ctrl);
    thread_ctrl->addSubs(left_view);
    thread_ctrl->addSubs(thread_ctrl);
    thread_ctrl->addSubs(replay_ctrl);

    // connect tasks to compose the graph
    left_model->addSubs(left_view);
    time_ctrl->addSubs(replay_ctrl);
    replay_ctrl->addSubs(left_model);
    left_model->addSubs(thread_ctrl); //add

    // run a thread for each task
    task = new thread(&View::run, left_view);
    task_list.push_back(task);
    task = new thread(&Model::run, left_model);
    task_list.push_back(task);
    task = new thread(&TimeCtrl::run, time_ctrl);
    task_list.push_back(task);
    task = new thread(&ThreadCtrl::run, thread_ctrl);
    task_list.push_back(task); //add
    task = new thread(&ReplayCtrl::run, replay_ctrl);
    task_list.push_back(task); //add

    // wait for each task to be terminated
    for (int i=0; i < task_list.size(); i++) 
      task_list[i]->join(); 

    close_screen();
  }
  else{
    cout<<"type valid number"<<endl;
  }
  return 0;
}
