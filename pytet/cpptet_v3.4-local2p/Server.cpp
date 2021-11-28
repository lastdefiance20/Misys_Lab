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

bool isGameDone = false;
bool returnName = false;
string endmsg;

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

void usage(char *argv){
    cout << "Usage : " << argv << " [port]" << endl;
    cout << "Example) " << argv << " 1234" << endl;
}

int main(int argc, char *argv[]){
    if(argc != 2){ // 인자가 2개가 아니면 usage 출력
        usage(argv[0]);
        return -1;
    }

    char buff[256]; // 읽기, 쓰기용 버퍼 선언

    struct sockaddr_in addr_server = {}; // 주소체계 구조체 선언
    struct sockaddr_in addr_client = {};
    socklen_t addr_client_len = sizeof(addr_client_len); // 길이 계산

    memset(&addr_server, 0, sizeof(addr_server)); // 초기화
    addr_server.sin_family = AF_INET; // IPv4 인터넷 프로토콜
    addr_server.sin_port = htons(atoi(argv[1])); // 첫번째 인자 PORT 지정
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

    while(1){
        string a;
        endmsg = a;

        printMsg("테트리스 서버입니다\n", win0);

        CTetris::init(setOfCBlockArrays, MAX_BLK_TYPES, MAX_BLK_DEGREES);
        
        isGameDone = false;
        returnName = false;

        int sock_client1 = accept(sock_server, (sockaddr*) &addr_client, &addr_client_len); // 연결 수락
        if(sock_client1 == -1){
            cout << "accept error" << endl;
            close(sock_server);
            exit(1);
        }
        printMsg("connected 1\n", win0);

        int sock_client2 = accept(sock_server, (sockaddr*) &addr_client, &addr_client_len); // 연결 수락
        if(sock_client2 == -1){
            cout << "accept error" << endl;
            close(sock_server);
            exit(1);
        }
        printMsg("connected 2\n", win0);

        map<char,char> dic1={{'q','q'},{'w','w'},{'a','a'},{'s','s'},{'d','d'},{' ',' '},{'y','s'}};

        View th_view1("Viewer 1", win0);
        th_view1.setwindow(win1);

        View th_view2("Viewer 2", win0);
        th_view2.setwindow(win2);

        Model th_model1("player 1", win0);
        th_model1.setkeypad(dic1);
        th_model1.addObserver(&th_view1);
        th_model1.setrefree();

        Model th_model2("player 2", win0);
        th_model2.setkeypad(dic1);
        th_model2.addObserver(&th_view2);
        th_model2.setrefree();

        th_model1.addObserver(&th_model2);
        th_model2.addObserver(&th_model1);

        RecvController th_recv1("Recv 1", win0);
        th_recv1.addObserver(&th_model1);
        th_recv1.addclient(sock_client1);
        th_recv1.setserver();

        RecvController th_recv2("Recv 2", win0);
        th_recv2.addObserver(&th_model2);
        th_recv2.addclient(sock_client2);
        th_recv2.setserver();

        SendController th_send1("Send 1", win0);
        th_send1.addclient(sock_client1);
        th_send1.setserver();
        
        SendController th_send2("Send 2", win0);
        th_send2.addclient(sock_client2);
        th_send2.setserver();

        th_recv1.addObserver(&th_send2);
        th_recv2.addObserver(&th_send1);

        th_model1.addObserver(&th_send1);
        th_model2.addObserver(&th_send2);

        vector<thread> threads;
        threads.push_back(thread(&View::run, &th_view1));
        threads.push_back(thread(&View::run, &th_view2));    
        threads.push_back(thread(&Model::run, &th_model1));
        threads.push_back(thread(&Model::run, &th_model2));
        threads.push_back(thread(&RecvController::run, &th_recv1));
        threads.push_back(thread(&RecvController::run, &th_recv2));
        threads.push_back(thread(&SendController::run, &th_send1));
        threads.push_back(thread(&SendController::run, &th_send2));

        for(int i = 0; i < 8; i++){
            threads[i].join();
        }

        wclear(win0);
        wclear(win1);
        wclear(win2);

        wrefresh(win0);
        wrefresh(win1);
        wrefresh(win2);
    }

    close(sock_server); // 연결 종료
    endwin();
    return 0;
}