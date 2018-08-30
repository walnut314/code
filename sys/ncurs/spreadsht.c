#include <ncurses.h>
#include <signal.h>
#include <stdlib.h>

void intro(int version);
void spreadsheet();
void initinput();
int passwdok(char *passwd);
void getfileandpw(char *filename);


int main() {
    void die();
    initscr();
    spreadsheet();
    while (1) ;
    die();
}

void die() {
    signal(SIGINT, SIG_IGN);
    mvcur(0, COLS-1, LINES-1, 0);
    endwin();
    exit(0);
}

#define VERSION 12

void spreadsheet() {
    char filename[20];

    intro(VERSION);
    //getfileandpw(filename);
}

void intro(int version) {
    int x1, x2, y;
    box(stdscr, '*', '-');
    move(1,2);
    addstr("SHOWME Spreadsheet Program, version: ");
    getyx(stdscr, y, x1);
    printw("%d", version);
    getyx(stdscr, y, x2);
    while (x2++ - x1 < 6)
        addch(' ');
    addstr("1/1/86");
    refresh();
}

void initinput() {
    raw();
    echo();
    nonl();
}

void getfileandpw(char *filename) {
    char passwd[6];
    int i = 0;
    mvaddstr(4,2, "What file do you want? ");
    refresh();
    while ((filename[i] = getch()) != '\r') i++;
    filename[i] = '\0';
    mvaddstr(3, 2, "Six character Password? ");
    noecho();
    refresh();
    i = 0;
    while (i < 6) passwd[i++] = getch();
    if (passwdok(passwd)) {
        mvaddstr(4, 26, "OK");
        /* readinfile(filename); */
        refresh();
    } else {
        mvaddstr(4, 26, "NO");
        refresh();
        die();
    }
}

int passwdok(char *passwd) {
    return (1);
}

int __top, __left;

void makefields() {
    WINDOW *worksp, *field[10][10];
    int i, j;
    extern int __left, __top;

    worksp = subwin(stdscr, LINES-2, COLS-2, 1, 1);
    werase(worksp);
    wmove(worksp, 1, 0);
    waddstr(worksp, "\t0\t1\t2\t3\t4\t5\t6\t7");
    wmove(worksp, 3, 0);
    waddch(worksp, 'a');
    mvwaddch(worksp,  5, 0, 'b');
    mvwaddch(worksp,  7, 0, 'b');
    mvwaddch(worksp,  9, 0, 'b');
    mvwaddch(worksp, 11, 0, 'b');
    mvwaddch(worksp, 13, 0, 'b');
    mvwaddch(worksp, 15, 0, 'b');
    mvwaddch(worksp, 17, 0, 'b');
    mvwaddch(worksp, 19, 0, 'b');
    mvwaddch(worksp, 21, 0, 'b');
    wrefresh(worksp);
    for (i = 0; i < 10; i++)
        for (j = 0; j < 10; j++) {
            field[i][j] = newwin(1, 7, 4+2*i, 7+8*j);
            wprintw(field[i][j], "%d %d", i, j);
        }
    __left = 0;
    __top = 0;
    //reffields(field);

}

