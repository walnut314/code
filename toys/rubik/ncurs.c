#include <ncurses_dll.h>
#include <stdlib.h>

void printing();
void moving_and_sleeping();
void colouring();

int main(void)
{
    initscr();

    addstr("-----------------\n| codedrome.com |\n| ncurses Demo  |\n-----------------\n\n");
    refresh();

    //printing();

    //moving_and_sleeping();

    //colouring();

    addstr("\npress any key to exit...");
    refresh();
    getch();
    endwin();

    return EXIT_SUCCESS;
}

