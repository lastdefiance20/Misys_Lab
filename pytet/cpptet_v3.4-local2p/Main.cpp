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

#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

//include code
#include <CTetris.h>
#include <ClassSetup.h>
#include <View.h>
#include <Model.h>
#include <SendController.h>
#include <RecvController.h>

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
string endmsg;
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

/**************************************************************/
/******************** Tetris Main Loop ************************/
/**************************************************************/
class TimeController: public KeyPublisher{
  public:
    Model* observers[2];
    WINDOW* prwin;
    int nobservers = 0;
    string cname;

    //컨트롤러 이름 설정
    TimeController(string name, WINDOW *win){
      cname = name;
      prwin = win;
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
      printMsg(cname, prwin);
    }
};

//키컨트롤러가 중간에 빠져나가는 방법이 필요하다
class KeyController: public KeyPublisher{
  public:
    Model* observers[2];
    int nobservers = 0;
    string cname;
    WINDOW* prwin;

    //컨트롤러 이름, 프린트 설정
    KeyController(string name, WINDOW *win){
      cname = name;
      prwin = win;
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
      printMsg(cname, prwin);
    }
};

int main(int argc, char *argv[]) {
  int dy, dx;
  char key = 0;

  //테트리스 창 크기 설정
  if (argc != 5) {
    cout << "ex) Main.exe 20 10 127.0.0.1 1234" << endl;
    cout << "dy, dx, IP, Port" << endl;
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
  
  printMsg("테트리스 2인용입니다. 상태 : ", win0);

  //initialize는 한번으로 충분함
  CTetris::init(setOfCBlockArrays, MAX_BLK_TYPES, MAX_BLK_DEGREES);

  //클라이언트 가동
  struct sockaddr_in addr_server = {}; // 주소체계 구조체 선언

  memset(&addr_server, 0, sizeof(addr_server));
  addr_server.sin_family = AF_INET; // IPv4 통신, 기본 인터넷 프로토콜 지정
  addr_server.sin_addr.s_addr = inet_addr(argv[3]); // 첫번째 인자 IP 대입
  addr_server.sin_port = htons(atoi(argv[4])); // 두번째 인자 PORT 대입

  int sock_client = socket(AF_INET, SOCK_STREAM, 0); // 소켓 생성(IPv4, TCP, 기본프로토콜)
  if(sock_client == -1){
      printMsg("socket error", win0);
      close(sock_client);
      endwin();
      exit(1);
  }

  if(connect(sock_client, (struct sockaddr*) &addr_server, sizeof(addr_server)) == -1){ // 연결 요청
      printMsg("connect error", win0);
      close(sock_client);
      endwin();
      exit(1);
  }

  //연결 확인
  printMsg("connected, ", win0);

  while(1){
    char buff[256];
    int read_chk = read(sock_client, buff, sizeof(buff)-1); // 읽기 버퍼사이즈-1 만큼 read(읽기)
    if (buff[0] == 'v') break;
  }

  printMsg("start\n", win0);

  //1p, 2p 플레이어 키패드
  map<char,char> dic1={{'q','q'},{'w','w'},{'a','a'},{'s','s'},{'d','d'},{' ',' '},{'y','s'}};
  map<char, char> dic2 = {{'u', 'q'}, {'i', 'w'}, {'j', 'a'}, {'k', 's'}, {'l', 'd'}, {'\r', ' '}, {'y', 's'}};

  View th_view1("Viewer 1", win0);
  th_view1.setwindow(win1);

  View th_view2("Viewer 2", win0);
  th_view2.setwindow(win2);

  Model th_model1("player 1", win0);
  th_model1.setkeypad(dic1);
  th_model1.addObserver(&th_view1);
  th_model1.setmainmodel();

  Model th_model2("player 2", win0);
  th_model2.setkeypad(dic1);
  th_model2.addObserver(&th_view2);

  th_model1.addObserver(&th_model2);
  th_model2.addObserver(&th_model1);

  RecvController th_recv("Recv 1", win0);
  th_recv.addObserver(&th_model2);
  th_recv.addclient(sock_client);

  SendController th_send("Send 1", win0);
  th_send.addclient(sock_client);
  th_send.setmain();

  KeyController th_cont1("keycontrol", win0);
	th_cont1.addObserver(&th_model1);

  TimeController th_cont2("timecontrol", win0);
	th_cont2.addObserver(&th_model1);

  th_model1.addObserver(&th_send);
  th_model2.addObserver(&th_send);

  vector<thread> threads;
  threads.push_back(thread(&View::run, &th_view1));
  threads.push_back(thread(&View::run, &th_view2));
  threads.push_back(thread(&SendController::run, &th_send));
  threads.push_back(thread(&RecvController::run, &th_recv));
  threads.push_back(thread(&Model::run, &th_model1));
  threads.push_back(thread(&Model::run, &th_model2));
  threads.push_back(thread(&KeyController::run, &th_cont1));
  threads.push_back(thread(&TimeController::run, &th_cont2));
  //https://stackoverflow.com/questions/10998780/stdthread-calling-method-of-class
  //이런 방식을 사용하면 따로 함수를 정의하지 않고 쓰레드로 넣을 수 있다
  //원하는 클래스의 멤버 함수와 클래스를 넣어주면 된다

  for(int i = 0; i < 8; i++){
    threads[i].join();
  }
  
  wclear(win0);

  wrefresh(win0);

  //진사람 출력(먼저 보드가 찬사람)
  string endMsg = "Program terminated! ";
  if(returnName == false){
    endMsg.append("I Win");
  }
  else{
    endMsg.append("I Lose");
  }
  printMsg(endMsg, win0);

  sleep(5);
  close(sock_client);
  endwin();
  //static 변수는 같은 주소 공유, 한번만 삭제 or 삭제X!
}