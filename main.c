#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
 

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
    printf("\e[1;1H\e[2J");     // scroll to the new display

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
        printf("\n");
    }
    printf("zqsd to move | Ctrl+c to quit\n");
}


void intro()
// intro of the game.
{
    char command;
    while (command != '\n') {
        printf("\e[1;1H\e[2J");
        printf("Cnake ©\n\n\n");
        printf("Hello little one, the mighty patapon give to you the most important quest :\n");
        printf("collect as much ka-chink as possible to make the patapon army grow !\n\n");
        printf("press 'enter' to continue\n");
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
    intro();

    srand(time(0)); // for random numbers

    static struct termios oldt, newt;       // for instant input
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);


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

    display(grid, p1, k);

    while(isAlive) {     // the command entered by user
        int isInput = 0;    // bool to check if input

        int newX = p1->x;   // new coor, will be modified by user input
        int newY = p1->y;

        char command;   // for user inputs

        while (isInput == 0) {

            command=getchar();

            if (command == 'z' && SNAKE_DIR != 'd') {       // 'z' to go up
                newX = p1->x-1;
                SNAKE_DIR = 'u';
                isInput=1;
                break;
            }
            else if (command == 's' && SNAKE_DIR != 'u') {   // 's' to go down
                newX = p1->x+1;
                SNAKE_DIR = 'd';
                isInput=1;
                break;
            }
            else if (command == 'q' && SNAKE_DIR != 'r') {   // 'q' to go left
                newY = p1->y-1;
                SNAKE_DIR = 'l';
                isInput=1;
                break;
            }
            else if (command == 'd' && SNAKE_DIR != 'l') {   // 'd' to go right
                newY = p1->y+1;
                SNAKE_DIR = 'r';
                isInput=1;
                break;
            }
        }

        
        snake *lastBeforeMove;  // variable to get the tail

        if (grid[newX][newY] != '#') {   // verify if new coor is correct
            if (len_snake(p1) > 2) {
                if (!is_snake(get_head(p1)->x, get_head(p1)->y, p1->next->next)) {
                    lastBeforeMove = get_tail(p1);   // get the tail of the snake (for the grow)
                    move_snake(newX, newY, p1);
                }
                else {
                    break;
                }
            }
            else {
                lastBeforeMove = get_tail(p1);   // get the tail of the snake (for the grow)
                move_snake(newX, newY, p1);
            }
            
        }
        else {      // else game over
            while (1) {
                printf("\e[1;1H\e[2J");     // scroll to the new display
                printf("Game Over !\n");                // game over screen
                printf("Your score : %d\n\n", len_snake(p1)-1);
                printf("press 'r' to restart or 'q' to quit\n");

                char command=getchar();
                if (command == 'r') {
                    free_snake(p1);     // free memory taken by snake
                    main();
                }
                else if (command == 'q') {
                    free_snake(p1);     // free memory taken by snake
                    printf("\e[1;1H\e[2J");     // scroll to the new display
                    exit(1);
                }
            }
            break;
        }

        if (p1->x == k->x && p1->y == k->y) {   // if snake on ka-ching
            grow_snake(lastBeforeMove->x, lastBeforeMove->y, p1);
            move_kaching(k, p1);
        }
        if (len_snake(p1) == ((SIZE-2)*(SIZE-2))) {
            while (1) {
                printf("\e[1;1H\e[2J");     // scroll to the new display
                printf("Great job ! the patapon army is now ready to kick some ass\n\n");     // win screen
                printf("press 'r' to restart or 'q' to quit\n");

                char command=getchar();
                if (command == 'r') {
                    free_snake(p1);     // free memory taken by snake
                    main();
                }
                else if (command == 'q') {
                    free_snake(p1);     // free memory taken by snake
                    printf("\e[1;1H\e[2J");     // scroll to the new display
                    exit(1);
                }
            }
            break;
        }

        display(grid, p1, k);   // display the game
        
    }
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);

    return 0;
}