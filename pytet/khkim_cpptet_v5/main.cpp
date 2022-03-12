#include <unistd.h>
#include <string>
#include <thread>
#include <vector>

#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <map>

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

#include "SendCtrl.h"
#include "RecvCtrl.h"

#include "WLCtrl.h"
#include "Umpire.h"
#include "FlowCtrl.h"

#include "IdCtrl.h"

/**************************************************************/
/**************** Tetris Blocks Definitions *******************/
/**************************************************************/
#define MAX_BLK_TYPES 7
#define MAX_BLK_DEGREES 4

//T%d번으로 블럭의 타입을 설정하고, D%d번으로 오른쪽으로 90도씩 돌린 블럭의 state를 표현한다

//순서바꾸기

//T0 = I shape
Array2D T0D0 = {{ 0, 0, 1, 0 }, 
                { 0, 0, 1, 0 },
                { 0, 0, 1, 0 },
                { 0, 0, 1, 0 }}; //I

Array2D T0D1 = {{ 0, 0, 0, 0 }, 
                { 0, 0, 0, 0 },
                { 1, 1, 1, 1 },
                { 0, 0, 0, 0 }};

Array2D T0D2 = {{ 0, 1, 0, 0 }, 
                { 0, 1, 0, 0 },
                { 0, 1, 0, 0 },
                { 0, 1, 0, 0 }};

Array2D T0D3 = {{ 0, 0, 0, 0 }, 
                { 1, 1, 1, 1 },
                { 0, 0, 0, 0 },
                { 0, 0, 0, 0 }};

//T1 = J shape
Array2D T1D0 = {{ 1, 0, 0 }, 
                { 1, 1, 1 },
                { 0, 0, 0 }};
Array2D T1D1 = {{ 0, 1, 1 }, 
                { 0, 1, 0 },
                { 0, 1, 0 }};
Array2D T1D2 = {{ 0, 0, 0 }, 
                { 1, 1, 1 },
                { 0, 0, 1 }};
Array2D T1D3 = {{ 0, 1, 0 }, 
                { 0, 1, 0 },
                { 1, 1, 0 }};

//T2 = L shape
Array2D T2D0 = {{ 0, 0, 1 }, 
                { 1, 1, 1 },
                { 0, 0, 0 }};
Array2D T2D1 = {{ 0, 1, 0 },
                { 0, 1, 0 }, 
                { 0, 1, 1 }};
Array2D T2D2 = {{ 0, 0, 0 }, 
                { 1, 1, 1 }, 
                { 1, 0, 0 }};
Array2D T2D3 = {{ 1, 1, 0 },
                { 0, 1, 0 }, 
                { 0, 1, 0 }};

//T3 = O shape
Array2D T3D0 = {{ 1, 1 },
                { 1, 1 }};
Array2D T3D1 = {{ 1, 1 },
                { 1, 1 }};
Array2D T3D2 = {{ 1, 1 },
                { 1, 1 }};
Array2D T3D3 = {{ 1, 1 },
                { 1, 1 }};

//T4 = S shape
Array2D T4D0 = {{ 0, 1, 1 }, 
                { 1, 1, 0 }, 
                { 0, 0, 0 }};
Array2D T4D1 = {{ 0, 1, 0 }, 
                { 0, 1, 1 },
                { 0, 0, 1 }};
Array2D T4D2 = {{ 0, 0, 0 }, 
                { 0, 1, 1 }, 
                { 1, 1, 0 }};
Array2D T4D3 = {{ 1, 0, 0 }, 
                { 1, 1, 0 },
                { 0, 1, 0 }};

//T5 = T shape
Array2D T5D0 = {{ 0, 1, 0 }, 
                { 1, 1, 1 },
                { 0, 0, 0 }};
Array2D T5D1 = {{ 0, 1, 0 }, 
                { 0, 1, 1 },
                { 0, 1, 0 }};
Array2D T5D2 = {{ 0, 0, 0 }, 
                { 1, 1, 1 },
                { 0, 1, 0 }};
Array2D T5D3 = {{ 0, 1, 0 }, 
                { 1, 1, 0 },
                { 0, 1, 0 }};

//T6 = Z shape
Array2D T6D0 = {{ 1, 1, 0 }, 
                { 0, 1, 1 },
                { 0, 0, 0 }};
Array2D T6D1 = {{ 0, 0, 1 }, 
                { 0, 1, 1 },
                { 0, 1, 0 }};
Array2D T6D2 = {{ 0, 0, 0 }, 
                { 1, 1, 0 },
                { 0, 1, 1 }};
Array2D T6D3 = {{ 0, 1, 0 }, 
                { 1, 1, 0 },
                { 1, 0, 0 }};

//블럭을 한번씩 계속 rotate시킨 청사진을 setOfCBlockArrays에 저장하여 블럭이 가능한 모든 state를 표현
//ex) ㅗ가 저장된 setOfBlockArrays = T1D0 ~ T1D3 = [ㅗ, ㅏ, ㅜ, ㅓ], 총 7가지 블럭
Array2D setOfCBlockArrays[] = {
  T0D0, T0D1, T0D2, T0D3,
  T1D0, T1D1, T1D2, T1D3,
  T2D0, T2D1, T2D2, T2D3,
  T3D0, T3D1, T3D2, T3D3,
  T4D0, T4D1, T4D2, T4D3,
  T5D0, T5D1, T5D2, T5D3,
  T6D0, T6D1, T6D2, T6D3,
};

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
  char port[6];
  char ip[16];

  cout << "Choose a game mode you want" << endl;
  cout << "=======Local Gamemode=======" << endl;
  cout << "1 => Play 1P Tetris Game" << endl;
  cout << "2 => Replay Recent 1P Tetris Game" << endl;
  cout << "3 => Play local 2P Tetris Game" << endl;
  cout << "=======Online Gamemode=======" << endl;
  cout << "4 => Make 1p Tetris Server" << endl;
  cout << "5 => Join Tetris Game" << endl;
  cout << "6 => Make 2p Tetris Server" << endl;
  cout << "7 => Replay Recent 2P Tetris Game" << endl;
  cin >> num;

  map<char,char> key_set={{'q','q'},{'w','w'},{'a','a'},{'s','s'},{'d','d'},{' ',' '},{'y','s'}};

  if(num == 4){
    cout<<"port"<<endl;
    cin >> port;
  }
  if(num == 5){
    cout<<"ip, port"<<endl;
    cin >> ip;
    cin >> port;
  }
  if(num == 6){
    cout<<"port"<<endl;
    cin >> port;
  }

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

  if(num == 1){
    // create tasks to compose a graph
    View *left_view = new View(&left_win, &bttm_win, "left_view");
    Model *left_model = new Model(&bttm_win, "left_model");

    View *rght_view = new View(&rght_win, &bttm_win, "rght_view");
    Model *rght_model = new Model(&bttm_win, "rght_model");

    TimeCtrl *time_ctrl = new TimeCtrl(&bttm_win, "time_ctrl", 1);
    KbdCtrl *kbd_ctrl = new KbdCtrl(&bttm_win, "kbd_ctrl", key_set);
    RandomCtrl *random_ctrl = new RandomCtrl(&bttm_win, "random_ctrl");
    LogCtrl *log_ctrl = new LogCtrl(&bttm_win, "log_ctrl", "log.txt");

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

    thread_ctrl->addSubs(rght_view);
    thread_ctrl->addSubs(rght_model);

    // connect tasks to compose the graph
    left_model->addSubs(left_view);
    rght_model->addSubs(rght_view);

    left_model->addSubs(thread_ctrl); //add
    left_model->addSubs(random_ctrl);

    left_model->addSubs(rght_model);

    time_ctrl->addSubs(log_ctrl);
    kbd_ctrl->addSubs(log_ctrl);
    random_ctrl->addSubs(log_ctrl);

    log_ctrl->addSubs(left_model);

    // run a thread for each task
    task = new thread(&View::run, left_view);
    task_list.push_back(task);
    task = new thread(&Model::run, left_model);
    task_list.push_back(task);

    task = new thread(&View::run, rght_view);
    task_list.push_back(task);
    task = new thread(&Model::run, rght_model);
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
    // create tasks to compose a graph
    View *left_view = new View(&left_win, &bttm_win, "left_view");
    Model *left_model = new Model(&bttm_win, "left_model");
    TimeCtrl *time_ctrl = new TimeCtrl(&bttm_win, "time_ctrl", 0.2); // use 0.2 sec to make replay faster
    ReplayCtrl *replay_ctrl = new ReplayCtrl(&bttm_win, "replay_ctrl", "log.txt");

    //control thread
    ThreadCtrl *thread_ctrl = new ThreadCtrl(&bttm_win, "thread_ctrl");
    thread_ctrl->addSubs(left_model);
    thread_ctrl->addSubs(time_ctrl);
    thread_ctrl->addSubs(left_view);
    thread_ctrl->addSubs(thread_ctrl);
    thread_ctrl->addSubs(replay_ctrl);

    // connect tasks to compose the graph
    left_model->addSubs(left_view);
    left_model->addSubs(thread_ctrl); //for end thread
    time_ctrl->addSubs(replay_ctrl);
    replay_ctrl->addSubs(left_model);

    // run a thread for each task
    task = new thread(&View::run, left_view);
    task_list.push_back(task);
    task = new thread(&Model::run, left_model);
    task_list.push_back(task);
    task = new thread(&TimeCtrl::run, time_ctrl);
    task_list.push_back(task);
    task = new thread(&ThreadCtrl::run, thread_ctrl);
    task_list.push_back(task);
    task = new thread(&ReplayCtrl::run, replay_ctrl);
    task_list.push_back(task);

    // wait for each task to be terminated
    for (int i=0; i < task_list.size(); i++) 
      task_list[i]->join(); 

    close_screen();
  }
  else if(num == 3){
    cout<<"not ready"<<endl;
  }
  else if(num == 4){
    struct sockaddr_in addr_server = {}; // 주소체계 구조체 선언
    struct sockaddr_in addr_client = {};
    socklen_t addr_client_len = sizeof(addr_client_len); // 길이 계산

    memset(&addr_server, 0, sizeof(addr_server)); // 초기화
    addr_server.sin_family = AF_INET; // IPv4 인터넷 프로토콜
    addr_server.sin_port = htons(atoi(port)); // 첫번째 인자 PORT 지정
    addr_server.sin_addr.s_addr = htonl(INADDR_ANY); // Anybody
 
    int sock_server = socket(AF_INET, SOCK_STREAM, 0); // 소켓 생성
    if(sock_server == -1){
        cout << "socket error" << endl;
        close(sock_server);
        exit(1);
    }

    if(bind(sock_server, (sockaddr*) &addr_server, sizeof(addr_server)) == -1){ // 주소 지정
        cout << "bind error" << endl;
        close(sock_server);
        exit(1);
    }

    if(listen(sock_server, 3) == -1){ // 연결 대기
        cout << "listen error" << endl;
        close(sock_server);
        exit(1);
    }

    int sock_client1 = accept(sock_server, (sockaddr*) &addr_client, &addr_client_len); // 연결 수락
    if(sock_client1 == -1){
        cout << "accept error" << endl;
        close(sock_server);
        exit(1);
    }

    char w_buff[256];
    w_buff[0] = 'v';
    int write_chk = write(sock_client1, w_buff, 1);
    
    //printMsg("connected 1", win0);

    SendCtrl *send_ctrl = new SendCtrl(&bttm_win, "send_ctrl", sock_client1);
    RecvCtrl *recv_ctrl = new RecvCtrl(&bttm_win, "recv_ctrl", sock_client1);

    recv_ctrl->addSubs(send_ctrl);

    task = new thread(&SendCtrl::run, send_ctrl);
    task_list.push_back(task);
    task = new thread(&RecvCtrl::run, recv_ctrl);
    task_list.push_back(task);

    // wait for each task to be terminated
    for (int i=0; i < task_list.size(); i++) 
      task_list[i]->join(); 

    close_screen();
  }
  else if(num == 5){
    struct sockaddr_in addr_server = {}; // 주소체계 구조체 선언

    memset(&addr_server, 0, sizeof(addr_server));
    addr_server.sin_family = AF_INET; // IPv4 통신, 기본 인터넷 프로토콜 지정
    addr_server.sin_addr.s_addr = inet_addr(ip); // 첫번째 인자 IP 대입
    addr_server.sin_port = htons(atoi(port)); // 두번째 인자 PORT 대입

    int sock_client = socket(AF_INET, SOCK_STREAM, 0); // 소켓 생성(IPv4, TCP, 기본프로토콜)
    if(sock_client == -1){
        //printMsg("socket error", win0);
        close(sock_client);
        endwin();
        exit(1);
    }

    if(connect(sock_client, (struct sockaddr*) &addr_server, sizeof(addr_server)) == -1){ // 연결 요청
        //printMsg("connect error", win0);
        close(sock_client);
        endwin();
        exit(1);
    }

    while(1){
      char buff[256];
      int read_chk = read(sock_client, buff, sizeof(buff)-1); // 읽기 버퍼사이즈-1 만큼 read(읽기)
      if (buff[0] == 'v') break;
    }

    View *left_view = new View(&left_win, &bttm_win, "left_view");
    Model *left_model = new Model(&bttm_win, "left_model");

    View *rght_view = new View(&rght_win, &bttm_win, "rght_view");
    Model *rght_model = new Model(&bttm_win, "rght_model");

    TimeCtrl *time_ctrl = new TimeCtrl(&bttm_win, "time_ctrl", 1);
    KbdCtrl *kbd_ctrl = new KbdCtrl(&bttm_win, "kbd_ctrl", key_set);
    RandomCtrl *random_ctrl = new RandomCtrl(&bttm_win, "random_ctrl");

    LogCtrl *log_ctrl = new LogCtrl(&bttm_win, "log_ctrl", "log.txt");

    SendCtrl *send_ctrl = new SendCtrl(&bttm_win, "send_ctrl", sock_client);
    RecvCtrl *recv_ctrl = new RecvCtrl(&bttm_win, "recv_ctrl", sock_client);

    WLCtrl *wl_ctrl = new WLCtrl(&bttm_win, "wl_ctrl");

    //control thread
    ThreadCtrl *thread_ctrl = new ThreadCtrl(&bttm_win, "thread_ctrl");
    thread_ctrl->addSubs(left_model);
    thread_ctrl->addSubs(left_view);

    thread_ctrl->addSubs(rght_view);
    thread_ctrl->addSubs(rght_model);

    thread_ctrl->addSubs(time_ctrl);
    thread_ctrl->addSubs(kbd_ctrl);
    thread_ctrl->addSubs(random_ctrl);

    thread_ctrl->addSubs(log_ctrl);

    thread_ctrl->addSubs(recv_ctrl);
    thread_ctrl->addSubs(send_ctrl);
    thread_ctrl->addSubs(thread_ctrl);

    thread_ctrl->addSubs(wl_ctrl);

    // connect tasks to compose the graph
    left_model->addSubs(left_view);
    rght_model->addSubs(rght_view);

    left_model->addSubs(random_ctrl);
    left_model->addSubs(rght_model);

    rght_model->addSubs(left_model);

    time_ctrl->addSubs(log_ctrl);
    kbd_ctrl->addSubs(log_ctrl);
    random_ctrl->addSubs(log_ctrl);

    log_ctrl->addSubs(left_model);
    log_ctrl->addSubs(send_ctrl);

    recv_ctrl->addSubs(rght_model);
    recv_ctrl->addSubs(wl_ctrl);

    wl_ctrl->addSubs(send_ctrl);
    wl_ctrl->addSubs(thread_ctrl);

    // run a thread for each task
    task = new thread(&View::run, left_view);
    task_list.push_back(task);
    task = new thread(&Model::run, left_model);
    task_list.push_back(task);

    task = new thread(&View::run, rght_view);
    task_list.push_back(task);
    task = new thread(&Model::run, rght_model);
    task_list.push_back(task);

    task = new thread(&LogCtrl::run, log_ctrl);
    task_list.push_back(task);

    task = new thread(&TimeCtrl::run, time_ctrl);
    task_list.push_back(task);
    task = new thread(&KbdCtrl::run, kbd_ctrl);
    task_list.push_back(task);
    task = new thread(&ThreadCtrl::run, thread_ctrl);
    task_list.push_back(task); //add
    task = new thread(&RandomCtrl::run, random_ctrl);
    task_list.push_back(task); //add

    task = new thread(&WLCtrl::run, wl_ctrl);
    task_list.push_back(task); //add

    task = new thread(&SendCtrl::run, send_ctrl);
    task_list.push_back(task);
    task = new thread(&RecvCtrl::run, recv_ctrl);
    task_list.push_back(task);

    // wait for each task to be terminated
    for (int i=0; i < task_list.size(); i++) 
      task_list[i]->join(); 
    
    bttm_win.printw("\nTetris done");
    
    char w_buff[256];
    w_buff[0] = 'v';
    int write_chk = write(sock_client, w_buff, 1);
    
    while(1){
      char buff[256];
      int read_chk = read(sock_client, buff, sizeof(buff)-1); // 읽기 버퍼사이즈-1 만큼 read(읽기)
      if (buff[0] == 'v') break;
    }

    close_screen();
  }
  else if (num == 6){
    struct sockaddr_in addr_server = {}; // 주소체계 구조체 선언
    struct sockaddr_in addr_client = {};
    socklen_t addr_client_len = sizeof(addr_client_len); // 길이 계산

    memset(&addr_server, 0, sizeof(addr_server)); // 초기화
    addr_server.sin_family = AF_INET; // IPv4 인터넷 프로토콜
    addr_server.sin_port = htons(atoi(port)); // 첫번째 인자 PORT 지정
    addr_server.sin_addr.s_addr = htonl(INADDR_ANY); // Anybody
 
    int sock_server = socket(AF_INET, SOCK_STREAM, 0); // 소켓 생성
    if(sock_server == -1){
        cout << "socket error" << endl;
        close(sock_server);
        exit(1);
    }

    if(bind(sock_server, (sockaddr*) &addr_server, sizeof(addr_server)) == -1){ // 주소 지정
        cout << "bind error" << endl;
        close(sock_server);
        exit(1);
    }

    if(listen(sock_server, 3) == -1){ // 연결 대기
        cout << "listen error" << endl;
        close(sock_server);
        exit(1);
    }

    int sock_client1 = accept(sock_server, (sockaddr*) &addr_client, &addr_client_len); // 연결 수락
    if(sock_client1 == -1){
        cout << "accept error" << endl;
        close(sock_server);
        exit(1);
    }

    int sock_client2 = accept(sock_server, (sockaddr*) &addr_client, &addr_client_len); // 연결 수락
    if(sock_client2 == -1){
        cout << "accept error" << endl;
        close(sock_server);
        exit(1);
    }

    char w_buff[256];
    w_buff[0] = 'v';
    int write_chk = write(sock_client1, w_buff, 1);
    write_chk = write(sock_client2, w_buff, 1);
    //printMsg("connected 1", win0);

    SendCtrl *send_ctrl_1 = new SendCtrl(&bttm_win, "send_ctrl_1", sock_client1);
    SendCtrl *send_ctrl_2 = new SendCtrl(&bttm_win, "send_ctrl_1", sock_client2);
    RecvCtrl *recv_ctrl_1 = new RecvCtrl(&bttm_win, "recv_ctrl_2", sock_client1);
    RecvCtrl *recv_ctrl_2 = new RecvCtrl(&bttm_win, "recv_ctrl_2", sock_client2);

    View *left_view = new View(&left_win, &bttm_win, "left_view");
    Model *left_model = new Model(&bttm_win, "left_model");

    View *rght_view = new View(&rght_win, &bttm_win, "rght_view");
    Model *rght_model = new Model(&bttm_win, "rght_model");

    FlowCtrl *rght_flow_ctrl = new FlowCtrl(&bttm_win, "rght_flow_ctrl", '1');
    FlowCtrl *left_flow_ctrl = new FlowCtrl(&bttm_win, "left_flow_ctrl", '2');

    Umpire *umpire = new Umpire(&bttm_win, "Umpire");

    LogCtrl *rght_log_ctrl = new LogCtrl(&bttm_win, "rght_log_ctrl", "p1.txt");
    LogCtrl *left_log_ctrl = new LogCtrl(&bttm_win, "left_log_ctrl", "p2.txt");
    LogCtrl *cter_log_ctrl = new LogCtrl(&bttm_win, "cter_log_ctrl", "pp.txt");

    IdCtrl *rght_id_ctrl = new IdCtrl(&bttm_win, "right_id_ctrl", '1', false);
    IdCtrl *left_id_ctrl = new IdCtrl(&bttm_win, "left_id_ctrl", '2', false);

    //control thread
    ThreadCtrl *thread_ctrl = new ThreadCtrl(&bttm_win, "thread_ctrl");
    thread_ctrl->addSubs(send_ctrl_1);
    thread_ctrl->addSubs(send_ctrl_2);
    thread_ctrl->addSubs(recv_ctrl_1);
    thread_ctrl->addSubs(recv_ctrl_2);
    thread_ctrl->addSubs(left_view);
    thread_ctrl->addSubs(left_model);
    thread_ctrl->addSubs(rght_view);
    thread_ctrl->addSubs(rght_model);
    thread_ctrl->addSubs(rght_flow_ctrl);
    thread_ctrl->addSubs(left_flow_ctrl);
    thread_ctrl->addSubs(umpire);
    thread_ctrl->addSubs(rght_log_ctrl);
    thread_ctrl->addSubs(left_log_ctrl);
    thread_ctrl->addSubs(cter_log_ctrl);
    thread_ctrl->addSubs(rght_id_ctrl);
    thread_ctrl->addSubs(left_id_ctrl);
    thread_ctrl->addSubs(thread_ctrl);

    rght_model->addSubs(left_model);
    left_model->addSubs(rght_model);

    recv_ctrl_1->addSubs(send_ctrl_2);
    recv_ctrl_1->addSubs(rght_model);
    recv_ctrl_1->addSubs(rght_log_ctrl);
    rght_log_ctrl->addSubs(rght_id_ctrl);
    rght_log_ctrl->addSubs(thread_ctrl);
    rght_model->addSubs(rght_flow_ctrl);
    rght_model->addSubs(rght_view);
    rght_flow_ctrl->addSubs(umpire);
    rght_flow_ctrl->addSubs(send_ctrl_2);

    recv_ctrl_2->addSubs(send_ctrl_1);
    recv_ctrl_2->addSubs(left_model);
    recv_ctrl_2->addSubs(left_log_ctrl);
    left_log_ctrl->addSubs(left_id_ctrl);
    left_log_ctrl->addSubs(thread_ctrl);
    left_model->addSubs(left_flow_ctrl);
    left_model->addSubs(left_view);
    left_flow_ctrl->addSubs(umpire);
    left_flow_ctrl->addSubs(send_ctrl_1);

    rght_id_ctrl->addSubs(cter_log_ctrl);
    left_id_ctrl->addSubs(cter_log_ctrl);

    umpire->addSubs(rght_flow_ctrl);
    umpire->addSubs(left_flow_ctrl);

    task = new thread(&LogCtrl::run, cter_log_ctrl);
    task_list.push_back(task);

    task = new thread(&RecvCtrl::run, recv_ctrl_1);
    task_list.push_back(task);
    task = new thread(&LogCtrl::run, rght_log_ctrl);
    task_list.push_back(task);
    task = new thread(&IdCtrl::run, rght_id_ctrl);
    task_list.push_back(task);
    task = new thread(&Model::run, rght_model);
    task_list.push_back(task);
    task = new thread(&View::run, rght_view);
    task_list.push_back(task);
    task = new thread(&FlowCtrl::run, rght_flow_ctrl);
    task_list.push_back(task);
    task = new thread(&SendCtrl::run, send_ctrl_1);
    task_list.push_back(task);

    task = new thread(&RecvCtrl::run, recv_ctrl_2);
    task_list.push_back(task);
    task = new thread(&LogCtrl::run, left_log_ctrl);
    task_list.push_back(task);
    task = new thread(&IdCtrl::run, left_id_ctrl);
    task_list.push_back(task);
    task = new thread(&Model::run, left_model);
    task_list.push_back(task);
    task = new thread(&View::run, left_view);
    task_list.push_back(task);
    task = new thread(&FlowCtrl::run, left_flow_ctrl);
    task_list.push_back(task);
    task = new thread(&SendCtrl::run, send_ctrl_2);
    task_list.push_back(task);
    
    task = new thread(&Umpire::run, umpire);
    task_list.push_back(task);
    task = new thread(&ThreadCtrl::run, thread_ctrl);
    task_list.push_back(task);

    // wait for each task to be terminated
    for (int i=0; i < task_list.size(); i++) 
      task_list[i]->join(); 

    w_buff[0] = 'v';
    write_chk = write(sock_client1, w_buff, 1);
    write_chk = write(sock_client2, w_buff, 1);

    close_screen();
  }
  else if (num == 7){
    // create tasks to compose a graph
    View *left_view = new View(&left_win, &bttm_win, "left_view");
    Model *left_model = new Model(&bttm_win, "left_model");

    View *rght_view = new View(&rght_win, &bttm_win, "rght_view");
    Model *rght_model = new Model(&bttm_win, "rght_model");

    TimeCtrl *time_ctrl = new TimeCtrl(&bttm_win, "time_ctrl", 0.2); // use 0.2 sec to make replay faster
    ReplayCtrl *rght_replay_ctrl = new ReplayCtrl(&bttm_win, "rght_replay_ctrl", "p1.txt");
    ReplayCtrl *left_replay_ctrl = new ReplayCtrl(&bttm_win, "left_replay_ctrl", "p2.txt");
    ReplayCtrl *cter_replay_ctrl = new ReplayCtrl(&bttm_win, "cter_replay_ctrl", "pp.txt");
  
    IdCtrl *rght_id_ctrl = new IdCtrl(&bttm_win, "right_id_ctrl", '1', true);
    IdCtrl *left_id_ctrl = new IdCtrl(&bttm_win, "left_id_ctrl", '2', true);

    //control thread
    ThreadCtrl *thread_ctrl = new ThreadCtrl(&bttm_win, "thread_ctrl");
    
    thread_ctrl->addSubs(left_view);
    thread_ctrl->addSubs(left_model);
    thread_ctrl->addSubs(rght_view);
    thread_ctrl->addSubs(rght_model);
    thread_ctrl->addSubs(time_ctrl);
    thread_ctrl->addSubs(rght_replay_ctrl);
    thread_ctrl->addSubs(left_replay_ctrl);
    thread_ctrl->addSubs(cter_replay_ctrl);
    thread_ctrl->addSubs(rght_id_ctrl);
    thread_ctrl->addSubs(left_id_ctrl);

    thread_ctrl->addSubs(thread_ctrl);

    // connect tasks to compose the graph
    time_ctrl->addSubs(cter_replay_ctrl);
    cter_replay_ctrl->addSubs(rght_id_ctrl);
    cter_replay_ctrl->addSubs(left_id_ctrl);

    rght_id_ctrl->addSubs(rght_replay_ctrl);
    left_id_ctrl->addSubs(left_replay_ctrl);

    rght_replay_ctrl->addSubs(rght_model);
    left_replay_ctrl->addSubs(left_model);

    rght_model->addSubs(rght_view);
    rght_model->addSubs(thread_ctrl);
    left_model->addSubs(left_view);
    left_model->addSubs(thread_ctrl); //for end thread

    rght_model->addSubs(left_model);
    left_model->addSubs(rght_model);

    // run a thread for each task
    task = new thread(&View::run, left_view);
    task_list.push_back(task);
    task = new thread(&Model::run, left_model);
    task_list.push_back(task);
    task = new thread(&View::run, rght_view);
    task_list.push_back(task);
    task = new thread(&Model::run, rght_model);
    task_list.push_back(task);
    task = new thread(&ReplayCtrl::run, rght_replay_ctrl);
    task_list.push_back(task);
    task = new thread(&ReplayCtrl::run, left_replay_ctrl);
    task_list.push_back(task);
    task = new thread(&ReplayCtrl::run, cter_replay_ctrl);
    task_list.push_back(task);
    task = new thread(&IdCtrl::run, rght_id_ctrl);
    task_list.push_back(task);
    task = new thread(&IdCtrl::run, left_id_ctrl);
    task_list.push_back(task);
    task = new thread(&TimeCtrl::run, time_ctrl);
    task_list.push_back(task);
    task = new thread(&ThreadCtrl::run, thread_ctrl);
    task_list.push_back(task);

    // wait for each task to be terminated
    for (int i=0; i < task_list.size(); i++) 
      task_list[i]->join(); 

    close_screen();
  }
  else{
    cout<<"invalid number"<<endl;
  }
  return 0;
}
