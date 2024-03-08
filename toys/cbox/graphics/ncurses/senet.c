/* senet.c */

#include <stdlib.h>
#include <ncurses.h>

#define SQ_HEIGHT 5
#define SQ_WIDTH 8

void create_board(void);
void destroy_board(void);

void draw_square(int sq);
void highlight_square(int sq);

WINDOW *BOARD[30];

int main(int argc, char **argv)
{
    int key;
    int sq;

    /* initialize curses */

    initscr();
    noecho();
    cbreak();

    if ((LINES < 24) || (COLS < 80)) {
        endwin();
        puts("Your terminal needs to be at least 80x24");
        exit(2);
    }

    /* print welcome text */

    clear();

    mvprintw(LINES - 1, (COLS - 5) / 2, "Senet");
    refresh();


    /* draw board */

    create_board();

    /* loop: '+' to increment squares, '-'
       to decrement squares */

    sq = 0;
    highlight_square(sq);

    do {
        key = getch();

        switch (key) {
        case '+':
        case '=':
            if (sq < 29) {
                draw_square(sq);
                highlight_square(++sq);
            }
            break;

        case '-':
        case '_':
            if (sq > 0) {
                draw_square(sq);
                highlight_square(--sq);
            }
        }
    } while ((key != 'q') && (key != 'Q'));

    /* when done, free up the board, and exit */

    destroy_board();

    endwin();
    exit(0);
}

void create_board(void)
{
    int i;
    int starty, startx;

    starty = 0;
    for (i = 0; i < 10; i++) {
        startx = i * SQ_WIDTH;
        BOARD[i] = newwin(SQ_HEIGHT, SQ_WIDTH, starty, startx);
    }

    starty = SQ_HEIGHT;
    for (i = 10; i < 20; i++) {
        startx = (19 - i) * SQ_WIDTH;
        BOARD[i] = newwin(SQ_HEIGHT, SQ_WIDTH, starty, startx);
    }

    starty = 2 * SQ_HEIGHT;
    for (i = 20; i < 30; i++) {
        startx = (i - 20) * SQ_WIDTH;
        BOARD[i] = newwin(SQ_HEIGHT, SQ_WIDTH, starty, startx);
    }


    /* put border on each window and refresh */

    for (i = 0; i < 30; i++) {
        draw_square(i);
    }
}

void destroy_board(void)
{
    int i;

    /* erase every box and delete each window */

    for (i = 0; i < 30; i++) {
        wborder(BOARD[i], ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
        wrefresh(BOARD[i]);

        delwin(BOARD[i]);
    }
}

void draw_square(int sq)
{
    switch (sq) {
    case 14:                    /* revive square */
        wborder(BOARD[sq], '#', '#', '#', '#', '#', '#', '#', '#');
        break;

    case 25:                    /* stop square */
        box(BOARD[sq], 'X', 'x');
        break;

    case 26:                    /* water square */
        box(BOARD[sq], 'O', 'o');
        break;

    case 27:                    /* 3-move square */
        box(BOARD[sq], '3', '3');
        break;

    case 28:                    /* 2-move square */
        box(BOARD[sq], '2', '2');
        break;

    default:
        box(BOARD[sq], 0, 0);
    }

    wrefresh(BOARD[sq]);
}

void highlight_square(int sq)
{
    wattron(BOARD[sq], A_BOLD);
    draw_square(sq);
    wattroff(BOARD[sq], A_BOLD);
}

