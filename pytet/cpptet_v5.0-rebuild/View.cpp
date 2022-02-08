#include <View.h>

View::View(string name, WINDOW *win){
    vname = name;
    prwin = win;
};

void View::update(Matrix* board){
    m.lock();
    boards.push(board);
    cv.notify_all();
    m.unlock();
    return;
};

void View::setwindow(WINDOW *x){
    drawwin = x;
};

void View::run(){
    while(isGameDone == false){
        unique_lock<mutex> lk(m);
        if(boards.empty()) cv.wait(lk, [&]{return !boards.empty();});
        Matrix board = boards.front();
        boards.pop();
        lk.unlock();
        drawScreen(board, drawwin);
    }
};


void View::drawScreen(Matrix board, WINDOW *win){
  int dy = board.get_dy();
  int dx = board.get_dx();
  int **array = board.get_array();
  wclear(win);

  for (int y = 0; y < dy; y++) {
    for (int x = 0; x < dx; x++) {
      if (array[y][x] == 0){
        mvwaddstr(win, y, x, "□");
      }
      else if (array[y][x] < 8){
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