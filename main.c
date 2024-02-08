#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <curses.h>

int SIZE = 10;
char SNAKE_DIR;


typedef struct snake snake;
struct snake {
    snake *prev;
    int x;
    int y;
    snake *next;
};


typedef struct kaching kaching;
struct kaching {
    int x;
    int y;
};


snake* new_snake(snake *prev, int x, int y, snake *next)
// add a snake
{
    snake *el;

    el = malloc(sizeof(snake));
    el->x = x;
    el->y = y;
    el->prev = prev;
    el->next = next;
    return el;
}


int len_snake(snake *pOG)
{
    snake *p = pOG;
    int len = 0;
    while (p!= NULL) {
        len++;
        p = p->next;
    }
    return len;
}


void grow_snake(int x, int y, snake *pOG)
// make the snake grow by adding one more cell.
{
    snake *p = pOG;
    while(p->next != NULL) {
        p = p->next;
    }
    snake *newP;
    newP = new_snake(p, x, y, NULL);
    p->next = newP;
}


void prepare_grid(char grid[SIZE][SIZE])
// prepare the grid with walls
{
    for (int i=0; i<SIZE; i++) {
        for (int j=0; j<SIZE; j++) {

            if (i==0 || i==SIZE-1) {        // if up or bottom wall
                grid[i][j] = '#';
            }
            else if (j==0 || j==SIZE-1) {   // if left or right wall
                grid[i][j] = '#';
            }
            else {                          // if empty space
                grid[i][j] = '.';
            }
        }
    }
}


int is_snake(int x, int y, snake *pOG)
// return 1 if (solid)snake is found at [x,y] coor, else 0.
{
    snake *p = pOG;
    while (p != NULL) {
        if (p->x == x && p->y == y) {
            return 1;
        }
        p = p->next;
    }
    return 0;
}


snake* get_tail(snake *pOG)
// return the tail of the snake.
{
    snake *p = pOG;
    while (p->next != NULL) {
        p = p->next;
    }
    return p;
}


snake* get_head(snake *pOG)
// return the head of the snake.
{
    snake *p = pOG;
    while (p->prev != NULL) {
        p = p->prev;
    }
    return p;
}


void move_snake(int newX, int newY, snake *pOG)
// move snake to a new coor.
{
    
    snake *p = get_tail(pOG);

    while (p->prev != NULL) {
        p->x = p->prev->x;
        p->y = p->prev->y;
        
        p = p->prev;
    }
    pOG->x = newX;
    pOG->y = newY;
}

void move_kaching(kaching *k, snake *p)
// move the kaching to new coor, can't be in snake
{
    while (is_snake(k->x, k->y, p)) {       // generate random coor until not in snake
        k->x = (rand() % (SIZE-2)) + 1;
        k->y = (rand() % (SIZE-2)) + 1;
    }
}
 

void display(char grid[SIZE][SIZE], snake *p, kaching *k)
// display the game
{
    printf("\e[1;1H\e[2J\r");     // scroll to the new display

    for (int i=0; i<SIZE; i++) {
        for (int j=0; j<SIZE; j++) {

            if (is_snake(i, j, p)) {                // snake
                if (get_head(p)->x == i && get_head(p)->y == j) {   // only if head -> <>v^
                    if (SNAKE_DIR == 'u') {
                        printf("▲ ");
                    }
                    else if (SNAKE_DIR == 'd') {
                        printf("▼ ");
                    }
                    else if (SNAKE_DIR == 'l') {
                        printf("◀ ");
                    }
                    else if (SNAKE_DIR == 'r') {
                        printf("▶ ");
                    }
                    else {
                        printf("◆ ");
                    }
                }
                else {                                              // else -> *
                    printf("▢ ");
                }
            }
            else if (k->x == i && k->y == j) {      // kaching
                printf("🄚 ");
            }
            else {                                  // the grid background
                printf("%c ",grid[i][j]);
            }
        }
        printf("\n\r");
    }
    printf("zqsd to move | Ctrl+c to quit\n\r");
}


void intro()
// intro of the game.
{
    char command;
    while (command != '\n') {
        printf("\e[1;1H\e[2J");
        printf("Cnake ©\n\n\n\r");
        printf("Hello little one, the mighty patapon give to you the most important quest :\n\r");
        printf("collect as much ka-chink as possible to make the patapon army grow !\n\n\r");
        printf("press 'enter' to continue\n\r");
        command=getchar();
    }
}


void free_snake(snake *p)
// free a list.
{
    snake *tmp;
    while (p != NULL) {
        tmp = p->next;
        free(p);
        p = tmp;
    }
}


int main()
{
    intro();    // display the intro

    srand(time(0)); // for random numbers

    char grid[SIZE][SIZE];      // prepare the grid
    prepare_grid(grid);

    kaching *k = malloc(sizeof(kaching));   // prepare ka-ching
    k->x = (rand() % (SIZE-2)) + 1;
    k->y = (rand() % (SIZE-2)) + 1;

    snake *p1;    
    int randX = k->x;
    int randY = k->y;                          // prepare the player
    while (randX == k->x && randY == k->y) {
        randX = (rand() % (SIZE-2)) + 1;
        randY = (rand() % (SIZE-2)) + 1;
    }
    p1 = new_snake(NULL, randX, randY, NULL);
    SNAKE_DIR = 'N';



    int isAlive = 1;    // variable that indicate if the player is alive
    int pressed;        // for user inputs

    display(grid, p1, k);

    WINDOW* win = initscr();    // setup for inputs
    keypad(win, true);
    nodelay(win, true);

    int originalCooldown = 20;
    int cooldown = originalCooldown;  // cooldown before making the snake go forward
    int timeForSleep = 30000;
    int snakeSpeed = 0;     // snake speed (start at 0)

    while(isAlive) {     // the command entered by user
        int isInput = 0;

        int newX = p1->x;   // new coor, will be modified by user input
        int newY = p1->y;

        pressed = wgetch(win);

        if (pressed == 'z' && SNAKE_DIR != 'd') {        // 'z' to go up
            newX = p1->x-1;
            SNAKE_DIR = 'u';
            isInput = 1;
            while (cooldown > 0) {
                cooldown--;
                usleep(timeForSleep);
            }

        }
        else if (pressed == 's' && SNAKE_DIR != 'u') {      // 's' to go down
            newX = p1->x+1;
            SNAKE_DIR = 'd';
            isInput = 1;
            while (cooldown > 0) {
                cooldown--;
                usleep(timeForSleep);
            }
        }
        else if (pressed == 'q' && SNAKE_DIR != 'r') {      // 'q' to go left
            newY = p1->y-1;
            SNAKE_DIR = 'l';
            isInput = 1;
            while (cooldown > 0) {
                cooldown--;
                usleep(timeForSleep);
            }
        }
        else if (pressed == 'd' && SNAKE_DIR != 'l') {     // 'd' to go right
            newY = p1->y+1;
            SNAKE_DIR = 'r';
            isInput = 1;
            while (cooldown > 0) {
                cooldown--;
                usleep(timeForSleep);
            }
        }
        erase();

        if (cooldown == 0) {
            if (SNAKE_DIR == 'u') {
                newX = p1->x-1;
                isInput = 1;
            }
            else if (SNAKE_DIR == 'd') {
                newX = p1->x+1;
                isInput = 1;
            }
            else if (SNAKE_DIR == 'l') {
                newY = p1->y-1;
                isInput = 1;
            }
            else if (SNAKE_DIR == 'r') {
                newY = p1->y+1;
                isInput = 1;
            }
        }

        if ((isInput == 1 && cooldown == 0) || cooldown == 0) {
            if (originalCooldown - snakeSpeed > 0) {
                cooldown = originalCooldown - snakeSpeed;     // new cooldown, -snakeSpeed to speedup 
            }
            else {      // if cooldown too fast (originalCooldown - snakeSpeed <= 0)
                cooldown = 1;
            }
            snake *lastBeforeMove;  // variable to get the tail

            if (grid[newX][newY] != '#') {   // verify if new coor is correct
                if (len_snake(p1) > 2) {
                    if (!is_snake(get_head(p1)->x, get_head(p1)->y, p1->next->next)) {
                        lastBeforeMove = get_tail(p1);   // get the tail of the snake (for the grow)
                        move_snake(newX, newY, p1);
                    }
                    else {              // bite himself
                        isAlive = 0;
                    }
                }
                else {
                    lastBeforeMove = get_tail(p1);   // get the tail of the snake (for the grow)
                    move_snake(newX, newY, p1);
                }
                
            }
            else {              // touched a wall
                isAlive = 0;
            }

            if (isAlive == 0) {      // else game over
                while (1) {
                    printf("\e[1;1H\e[2J");     // scroll to the new display
                    printf("Game Over !\n\r");                // game over screen
                    printf("Your score : %d\n\n\r", len_snake(p1)-1);
                    printf("press 'r' to restart or 'q' to quit\n\r");

                    char command=getchar();
                    if (command == 'r') {
                        free_snake(p1);     // free memory taken by snake
                        endwin();
                        main();
                    }
                    else if (command == 'q') {
                        free_snake(p1);     // free memory taken by snake
                        printf("\e[1;1H\e[2J");     // scroll to the new display
                        endwin();
                        exit(1);
                    }
                }
            }

            if (p1->x == k->x && p1->y == k->y) {   // if snake on ka-ching
                grow_snake(lastBeforeMove->x, lastBeforeMove->y, p1);
                move_kaching(k, p1);
                snakeSpeed++;
            }
            if (len_snake(p1) == ((SIZE-2)*(SIZE-2))) {
                while (1) {
                    printf("\e[1;1H\e[2J");     // scroll to the new display
                    printf("Great job ! the patapon army is now ready to kick some ass\n\n\r");     // win screen
                    printf("press 'r' to restart or 'q' to quit\n\r");

                    char command=getchar();
                    if (command == 'r') {
                        free_snake(p1);     // free memory taken by snake
                        endwin();
                        main();
                        
                    }
                    else if (command == 'q') {
                        free_snake(p1);     // free memory taken by snake
                        printf("\e[1;1H\e[2J");     // scroll to the new display
                        endwin();
                        exit(1);
                    }
                }
            }
        }

        display(grid, p1, k);   // display the game
        refresh();
        cooldown--;
        //printf("cooldown : %d\n\r", cooldown);
        //printf("input : %d\n\r", isInput);
        usleep(timeForSleep);
    }

    return 0;
}