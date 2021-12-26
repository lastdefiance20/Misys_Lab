#include <View.h>

View::View(string name, WINDOW *win){
    vname = name;
    prwin = win;
};

void View::update(CTetris* board){
    m.lock();
    boards.push(board);
    cv.notify_all();
    m.unlock();
    return;
};

void View::setwindow(WINDOW *x){
    n = x;
};

void View::run(){
    while(isGameDone == false){
        unique_lock<mutex> lk(m);
        if(boards.empty()) cv.wait(lk, [&]{return !boards.empty();});
        CTetris* board = boards.front();
        boards.pop();
        lk.unlock();
        drawScreen(board, n);
    }
    /*
    string breaked = " breaked\n";
    vname.append(breaked);
    printMsg(vname, prwin);
    */
};