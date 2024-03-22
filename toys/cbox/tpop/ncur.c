#include <ncurses.h>


// gcc -lncurses ncur.c

int main()
{
    char ch;
    initscr();
    printw("hello, world");
    refresh();
    while ((ch=getch()) != 'q');
    endwin();
}

