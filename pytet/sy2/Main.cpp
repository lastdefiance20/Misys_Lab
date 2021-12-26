#include <iostream>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <termios.h>

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
using namespace std;

#include <vector>
#include <chrono>
#include "ModelView.h"
#include "SendRecv.h"
#include <thread>


char saved_key = 0;
int tty_cbreak(int fd);	/* put terminal into cbreak mode */
int tty_reset(int fd);	/* restore terminal's mode */

/* Read 1 character - echo defines echo mode */
char getChar() {
  char ch;
  int n;
  while (1) {
    tty_cbreak(0);
    n = read(0, &ch, 1);
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

int T3D0[] = { 1, 1, 1, 1, -1 }; //O
int T3D1[] = { 1, 1, 1, 1, -1 };
int T3D2[] = { 1, 1, 1, 1, -1 };
int T3D3[] = { 1, 1, 1, 1, -1 };

int T5D0[] = { 0, 1, 0, 1, 1, 1, 0, 0, 0, -1 }; //T
int T5D1[] = { 0, 1, 0, 0, 1, 1, 0, 1, 0, -1 };
int T5D2[] = { 0, 0, 0, 1, 1, 1, 0, 1, 0, -1 };
int T5D3[] = { 0, 1, 0, 1, 1, 0, 0, 1, 0, -1 };

int T1D0[] = { 1, 0, 0, 1, 1, 1, 0, 0, 0, -1 }; //J
int T1D1[] = { 0, 1, 1, 0, 1, 0, 0, 1, 0, -1 };
int T1D2[] = { 0, 0, 0, 1, 1, 1, 0, 0, 1, -1 };
int T1D3[] = { 0, 1, 0, 0, 1, 0, 1, 1, 0, -1 };

int T2D0[] = { 0, 0, 1, 1, 1, 1, 0, 0, 0, -1 }; //L
int T2D1[] = { 0, 1, 0, 0, 1, 0, 0, 1, 1, -1 };
int T2D2[] = { 0, 0, 0, 1, 1, 1, 1, 0, 0, -1 };
int T2D3[] = { 1, 1, 0, 0, 1, 0, 0, 1, 0, -1 };

int T6D0[] = { 1, 1, 0, 0, 1, 1, 0, 0, 0, -1 }; //Z
int T6D1[] = { 0, 0, 1, 0, 1, 1, 0, 1, 0, -1 };
int T6D2[] = { 0, 0, 0, 1, 1, 0, 0, 1, 1, -1 };
int T6D3[] = { 0, 1, 0, 1, 1, 0, 1, 0, 0, -1 };

int T4D0[] = { 0, 1, 1, 1, 1, 0, 0, 0, 0, -1 }; //S
int T4D1[] = { 0, 1, 0, 0, 1, 1, 0, 0, 1, -1 };
int T4D2[] = { 0, 0, 0, 0, 1, 1, 1, 1, 0, -1 };
int T4D3[] = { 1, 0, 0, 1, 1, 0, 0, 1, 0, -1 };

int T0D0[] = { 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, -1 }; //I
int T0D1[] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, -1 };
int T0D2[] = { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1 };
int T0D3[] = { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, -1 };

int *setOfCBlockArrays[] = {
  T0D0, T0D1, T0D2, T0D3,
  T1D0, T1D1, T1D2, T1D3,
  T2D0, T2D1, T2D2, T2D3,
  T3D0, T3D1, T3D2, T3D3,
  T4D0, T4D1, T4D2, T4D3,
  T5D0, T5D1, T5D2, T5D3,
  T6D0, T6D1, T6D2, T6D3,
};

bool isGameDone=false;
std::mutex mu;

class KeyController:public KeyPublisher{
    public:
        string name; 
        std::vector<Model*> observers;
	
        KeyController(string Name){
                name=Name; 
        }
        void addObserverKey(Model* input){
            observers.push_back(input);
        }
        void notifyObserversKey(char key){
            for(int i=0;i<observers.size();i++){
                observers[i]->updateKey(key);
            }
        }
        void run(){
            while(!isGameDone){
                try{
                char key=getChar();
                notifyObserversKey(key); 
                if(!key) throw runtime_error("key interrupt");
                } catch (runtime_error err){
                //printMsg("pass");
                }
            }
            sleep(1);
            notifyObserversKey(0);
        }
};

class TimeController:public KeyPublisher{ 
    public: 
        string name;
        std::vector<Model*> observers; 
        
	    TimeController(string Name){
            name=Name; 
        }
        void addObserverKey(Model* input){ 
            observers.push_back(input); 
        }
        void notifyObserversKey(char key){
            for(int i=0;i<observers.size();i++){ 
                observers[i]->updateKey(key);
            }
        }
        void run(){ 
            while(!isGameDone){
                sleep(1);
                notifyObserversKey('y');
            }
            sleep(1);
            notifyObserversKey(0); 
        }
};

void usage(char *argv){ 
    cout << "Usage : " << argv << " [ip] [port]" << endl;
    cout << "Example) " << argv << " 192.168.0.12 1234" << endl;
}
void signal_handler(int signal)
{
  cout<<" "<<endl;
}
int main(int argc, char *argv[]){
    if(argc!=3){
        usage(argv[0]);
        return -1;
    }
    signal(SIGTSTP,signal_handler);
    struct sockaddr_in addr_server = {};
    memset(&addr_server, 0, sizeof(addr_server));
    addr_server.sin_family = AF_INET; 
    addr_server.sin_addr.s_addr = inet_addr(argv[1]); 
    addr_server.sin_port = htons(atoi(argv[2])); 

    int sock_client = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_client == -1){ 
        cout << "socket error" << endl; 
        close(sock_client); 
        exit(1); 
    }
    bool option=1;
    setsockopt( sock_client, SOL_SOCKET, SO_REUSEADDR||SO_REUSEPORT, (const char*)&option, sizeof(option) );
    if(connect(sock_client, (struct sockaddr*) &addr_server, sizeof(addr_server)) == -1){ 
        cout << "connect error" << endl;
        close(sock_client);
        exit(1); 
    }

    setlocale(LC_ALL,""); //□, ■ 출력할 수 있게 함
    initscr(); //curses모드 시작 (curses를 사용하려면 반드시 초기화 해주어야 함)
    clear(); //창 지우기
    start_color(); //color사용할 수 있게 함
    init_pair(1,COLOR_RED,COLOR_BLACK);
    init_pair(2,COLOR_GREEN,COLOR_BLACK);
    init_pair(3,COLOR_YELLOW,COLOR_BLACK);
    init_pair(4,COLOR_BLUE,COLOR_BLACK);
    init_pair(5,COLOR_MAGENTA,COLOR_BLACK);
    init_pair(6,COLOR_CYAN,COLOR_BLACK);
    init_pair(7,COLOR_WHITE,COLOR_BLACK);
    CTetris::init(setOfCBlockArrays, MAX_BLK_TYPES, MAX_BLK_DEGREES); 
    WINDOW *win1,*win2;
    win1=newwin(20, 30, 0, 0); 
    win2=newwin(20, 30, 0, 60); 

    map<char,char> keypad1={{'q','q'},{'w','w'},{'a','a'},{'s','y'},{'d','d'},{' ',' '},{'y','y'},{-1,-1},{'0','0'},{'1','1'},{'2','2'},{'3','3'},{'4','4'},{'5','5'},{'6','6'}};

    bool canStart=false;
    while(!canStart){
        char buff[2];
        int read_chk=read(sock_client,buff,1);
        if(read_chk == -1) //read 실패 시,
            cout << "Please Wait..." << endl;
        if(buff[0]=='o') canStart=true;
    }

    View th_view1("view1"); 
    th_view1.addWindow(win1);
    View th_view2("view2"); 
    th_view2.addWindow(win2); 
    
    SendController scont("scont", false);
    scont.setClient(sock_client,0);

    Model th_model1("model1",true, false); 
    th_model1.addKeypad(keypad1);
    th_model1.addObserverView(&th_view1); 
    th_model1.addObserverKey(&scont);

    Model th_model2("model2",false, false); 
    th_model2.addKeypad(keypad1); 
    th_model2.addObserverView(&th_view2); 

    th_model1.addObserverDel(&th_model2);
    th_model2.addObserverDel(&th_model1);

    RecvController rcont("rcont",sock_client, false);
    rcont.addObserverKey(&th_model2);

    KeyController th_cont1("kcont"); 
    th_cont1.addObserverKey(&th_model1);

    TimeController th_cont2("tcont");
    th_cont2.addObserverKey(&th_model1);

	
    std::vector<std::thread> threads;
    threads.push_back(std::thread(&View::run, &th_view1)); 
    threads.push_back(std::thread(&View::run, &th_view2));
    threads.push_back(std::thread(&Model::run, &th_model1));
    threads.push_back(std::thread(&Model::run, &th_model2));
    threads.push_back(std::thread(&SendController::run, &scont));
    threads.push_back(std::thread(&RecvController::run, &rcont));
    threads.push_back(std::thread(&KeyController::run, &th_cont1));
    threads.push_back(std::thread(&TimeController::run, &th_cont2));

    for(int i=0;i<threads.size();i++){
        threads[i].join();
    }
    endwin();
    close(sock_client);
    return 0; 
}
