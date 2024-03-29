/*
 *
 *  g++ -std=c++14 space.cpp
 */

#include <iostream>
#include <ctime>
#include <unistd.h>
#include <thread>
#include <chrono>
using namespace std;
 
//#define CLEAR_SCR   ("cls")
#define CLEAR_SCR   ("\033[2J")

#define KEY_BULLET  (0)
#define KEY_LEFT    (1)
#define KEY_RIGHT   (2)
#define KEY_QUIT    (3)
#define KEY_HELP    (4)
#define KEY_INVALID (42)

#define BOARD_HEIGHT (20)
#define BOARD_WIDTH  (40)

const char SYMBOL_EMPTY  = ' ';
const char SYMBOL_PLAYER = '^';
const char SYMBOL_ALIEN  = 'A';
const char SYMBOL_WALL   = '#';
const char SYMBOL_BULLET = '*';
const int  GameSpeed     = 1000;

bool isBullet = false;

struct Player
{
    int x, y, startx, starty, score, lives;
    Player()
    {
        x = -1;
        y = -1;
        starty = 13;
        startx = (BOARD_WIDTH/2);
        score = 0;
        lives = 3;
    }
};
 
int n_bullets = 0;
struct Bullet
{
    int x, y;
    ~Bullet() {
    }
    Bullet(Player &player)
    {
        x = player.x;
        y = player.y-1;
        printf("bullet: x:%d y:%d\n", x, y);
    }
};
//Bullet bullets[256];

Player player;
Bullet *bullet;


char board[BOARD_HEIGHT][BOARD_WIDTH] =
{
    "#######################################",
    "#                                     #",
    "#                                     #",
    "#                                     #",
    "#                                     #",
    "#                                     #",
    "#                                     #",
    "#                                     #",
    "#                                     #",
    "#                                     #",
    "#                                     #",
    "#                                     #",
    "#                                     #",
    "#                                     #",
    "#######################################",
};
 
bool isValidPos(int x, int y)
{
    return (x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT);
}
 
bool moveBullet(Bullet &bullet, Player &player)
{
    if (!isBullet) return true;
    //printf("bullet: x:%d, y:%d\n", bullet.x, bullet.y);
    if(isValidPos(bullet.x, bullet.y+1))
    {
        char ch = board[bullet.y-1][bullet.x];
        //printf("bullet: is valid: %c\n", ch);
        if (ch == SYMBOL_EMPTY)
        {
            //printf("bullet: empty\n");
            board[bullet.y][bullet.x] = SYMBOL_EMPTY;
            bullet.y--;
            board[bullet.y][bullet.x] = SYMBOL_BULLET;
            return true;
        }
        else if (ch == SYMBOL_ALIEN)
        {
            //printf("bullet: alien\n");
            board[bullet.y][bullet.x] = SYMBOL_EMPTY;
            board[bullet.y+1][bullet.x] = SYMBOL_EMPTY;
        }
        else
        {
            //printf("bullet: no alien\n");
            board[bullet.y][bullet.x] = SYMBOL_EMPTY;
        }
    }
    return false;
}
 
void movePlayer(Player &player, int y, int x)
{
    printf("move: x:%d y:%d\n", x, y);
    if (!isValidPos(x, y))
    {
        return;
    }
     
    char ch = board[y][x];
 
    if(ch != SYMBOL_EMPTY)
    {
        return;
    }
     
    if (isValidPos(player.x, player.y))
    {
        board[player.y][player.x] = SYMBOL_EMPTY;
    }
    player.y = y; player.x = x;
    board[player.y][player.x] = SYMBOL_PLAYER;
    return;
}
 
bool isLevelFinished()
{
    return false;
    for (int y = 0; y < 10; y++)
    {
        for (int x = 0; x < 50; x++)
        {
            if (board[y][x] == SYMBOL_ALIEN)
            {
                return false;
            }
        }
    }
    return true;
}
 
void showPlayer(Player &player)
{
    cout << "\nPlayerX: " << player.x << endl;
    cout << "PlayerY: " << player.y << endl;
    cout << "Player's Score: " << player.score << endl;
    cout << "Player's Lives: " << player.lives << endl;
}
 
void showMap()
{
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        cout << board[y] << endl;
    }
}
 
void usage()
{
    printf("h: help\n");
    printf("b: bullet\n");
    printf("r: right\n");
    printf("l: left\n");
    printf("q: quit\n");
}

int getInput()
{
    char c = ' ';
retry:
    printf("> ");
    //while ((c = getchar()) != 'b') putchar(c);
    c = getchar();
    switch (c) {
        case 'h': usage();                return KEY_HELP;
        case 'b': /*printf("bullet\n");*/ return KEY_BULLET;
        case 'l': /*printf("left\n");  */ return KEY_LEFT;
        case 'r': /*printf("right\n"); */ return KEY_RIGHT;
        case 'q': /*printf("quit\n");  */ return KEY_QUIT;
    }
    printf("\r");
    goto retry;
}

void gameLoop()
{
    movePlayer(player, 13, BOARD_WIDTH/2);
    int input;
    while (!isLevelFinished() && player.lives > 0)
    {
        movePlayer(player, player.y, player.x);
        showPlayer(player);
        input = getInput();
        switch (input) {
            case KEY_BULLET:
                if(!isBullet) {
                    bullet = new Bullet(player); 
                    isBullet = true;
                }
                break;
            case KEY_LEFT:
                movePlayer(player, player.y, player.x-1);
                break;
            case KEY_RIGHT:
                movePlayer(player, player.y, player.x+1);
                break;
            case KEY_HELP:
            case KEY_INVALID:
                break;
            case KEY_QUIT:
                goto exit;
        }
    }
exit:
    cout << "Game Over!" << endl;
    cout << "Your final score was: " << player.score << endl;
}
 
void bullet_task()
{
    while (1) {
        if (!moveBullet(*bullet, player)) {
            isBullet = false;
            delete bullet;
        }
        system(CLEAR_SCR);
        showMap();
        std::this_thread::sleep_for(250ms);
    }
}

int main()
{
    std::thread t(bullet_task);
    t.detach();

    srand(time(0));
    //system ("/bin/stty raw");
    gameLoop();
    //system ("/bin/stty cooked");
    return 0;
}

