#include <ClassSetup.h>
#include <mutex>

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

mutex printm;
//win0 창에 종료메세지 등 메세지를 출력한다
void printMsg(string str, WINDOW *win){
  drawm.lock();
  endmsg.append(str);
  //string -> char으로 변환하여 printw가 출력 가능한 char 형식으로 변환해준다
  const char *message = endmsg.c_str();
  wclear(win);
  wprintw(win, message);
  wrefresh(win);
  drawm.unlock();
}

void printStateMsg(string str, WINDOW *win){
  statemsg.append(str);
  const char *message = statemsg.c_str();
  wclear(win);
  wprintw(win, message);
  drawm.lock();
  wrefresh(win);
  drawm.unlock();
}

int clientInitial[] = {1, -1, -1, -1};
//initial start -> Running
int clientRunning[] = {-1, 2, -1, 1};
//Running finished -> Receiving, 
int clientReceiving[] = {-1, -1, 3, -1};
//Receiving received -> Responding
int clientResponding[] = {-1, -1, -1, 4};
//responding update -> Finished
int clientFinished[] = {-1, -1, -1, -1};
//nothing

int *modelStateTransMatrix[]{
  clientInitial, clientRunning, clientReceiving,
  clientResponding, clientFinished
};

int serverWaiting[] = {1, -1, -1, -1, -1};
//waiting start -> Running
int serverRunning[] = {-1, 2, 3, -1, 1};
//Running finishedq -> TerminatingP, finishedM->TerminatingS
int serverTerminatingP[] = {-1, -1, -1, 0, -1};
//Receiving receive -> waiting
int serverTerminatingS[] = {-1, -1, -1, 0, -1};
//responding receive -> waiting

int *serverStateTransMatrix[]{
  serverWaiting, serverRunning, serverTerminatingP,
  serverTerminatingS
};

void drawClientState(){
  if(clientState == cInitial) printStateMsg("Initial\n", win3);
}

void drawClientState(ClientCommand cc){
  ClientState tempc = static_cast<ClientState>(modelStateTransMatrix[clientState][cc]);
  if(tempc != clientState){
    clientState = tempc;
    if(clientState == cError) printStateMsg("Error\n", win3);
    else if(clientState == cInitial) printStateMsg("Initial\n", win3);
    else if(clientState == cRunning) printStateMsg("Running\n", win3);
    else if(clientState == cReceiving) printStateMsg("Receiving\n", win3);
    else if(clientState == cResponding) printStateMsg("Responding\n", win3);
    else if(clientState == cFinished) printStateMsg("Finished\n", win3);
  }
};

void drawServerState(){
  if(serverState == sWaiting) printStateMsg("Waiting\n", win3);
}

void drawServerState(ServerCommand sc){
  ServerState temps = static_cast<ServerState>(serverStateTransMatrix[serverState][sc]);
  if(temps != serverState){
    serverState = temps;
    if(serverState == sError) printStateMsg("Error\n", win3);
    else if(serverState == sWaiting) printStateMsg("Waiting\n", win3);
    else if(serverState == sRunning) printStateMsg("Running\n", win3);
    else if(serverState == sTerminatingP) printStateMsg("TerminatingP\n", win3);
    else if(serverState == sTerminatingS) printStateMsg("TerminatingS\n", win3);
  }
};