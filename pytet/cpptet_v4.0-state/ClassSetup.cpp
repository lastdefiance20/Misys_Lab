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
  printm.lock();
  endmsg.append(str);
  //string -> char으로 변환하여 printw가 출력 가능한 char 형식으로 변환해준다
  const char *message = endmsg.c_str();
  wclear(win);
  wprintw(win, message);
  drawm.lock();
  wrefresh(win);
  drawm.unlock();
  printm.unlock();
}