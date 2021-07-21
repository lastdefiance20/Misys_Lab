#include <iostream>
#include <ncurses.h>
#include <string>

using namespace std;

WINDOW * win1;
WINDOW * win2;

void printman(){
    mvwprintw(win1, 0, 0, "refreshed me");
}

int main(){
    initscr();
    start_color();
    refresh();

    win1 = newwin(20, 30, 0, 0); 
    win2 = newwin(20, 30, 0, 40);

    mvwprintw(win2, 0, 0, "this is me");
    mvwprintw(win2, 0, 0, "this is you");
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