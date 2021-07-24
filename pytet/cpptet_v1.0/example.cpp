#include <iostream>
#include <ncursesw/ncurses.h>
#include <string>

using namespace std;

WINDOW * win1;
WINDOW * win2;

void printman(){
    mvwprintw(win1, 0, 0, "refreshed me");
}

int main(){
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    refresh();

    win1 = newwin(20, 30, 0, 0); 
    win2 = newwin(20, 30, 0, 40);

    wattron(win1, COLOR_PAIR(1));
    mvwprintw(win1,0,0,"□");
    wattroff(win1, COLOR_PAIR(1));

    attron(COLOR_PAIR(2));
    waddstr(win2,"□ins");
    attroff(COLOR_PAIR(2));

    wrefresh(win1);
    wrefresh(win2);
    getch();

    printman();
    wclear(win2);
    mvwprintw(win2, 0, 0, "refreshed you");

    wrefresh(win1);
    wrefresh(win2);

    getch();
    delwin(win1);
    delwin(win2);
    endwin();
    return 0;
}

/*
using namespace std;

int main()
{
    WINDOW *win;
    initscr();
    start_color();
    refresh();
    win = newwin(10, 10, 1, 1);
    mvwprintw(win, 1, 1, "A new window");
    wrefresh(win);
    getch();
    delwin(win);
    endwin();
}
*/