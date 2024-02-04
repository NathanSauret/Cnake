#include <stdio.h>
#include <stdlib.h>


int SIZE = 10;


typedef struct snake snake;
struct snake {
    int x;
    int y;
    snake *next;
};


typedef struct kaching kaching;
struct kaching {
    int x;
    int y;
};


snake* new_snake(int x, int y, snake *next)
// add a snake
{
    snake *el;

    el = malloc(sizeof(snake));
    el->x = x;
    el->y = y;
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
    newP = new_snake(x, y, NULL);
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


void move_snake(int newX, int newY, snake *pOG)
// move snake to a new coor.
{
    snake *p = pOG;

    while (p->next != NULL) {
        int x=

        p->next->x = p->x;
        p->next->y = p->y;

        p = p->next;
    }
    pOG->x = newX;
    pOG->y = newY;
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


void display(char grid[SIZE][SIZE], snake *p, kaching *k)
// display the game
{
    //printf("\e[1;1H\e[2J");     // scroll to the new display

    for (int i=0; i<SIZE; i++) {
        for (int j=0; j<SIZE; j++) {

            if (is_snake(i, j, p)) {
                printf("* ");
            }
            else if (k->x == i && k->y == j) {
                printf("@ ");
            }
            else {
                printf("%c ",grid[i][j]);
            }
        }
        printf("\n");
    }
    printf("zqsd to move | Ctrl+c to quit\n");
    printf("input = ");
}



int main()
{
    char grid[SIZE][SIZE];  // prepare the grid
    prepare_grid(grid);

    snake *p1;                  // prepare thr player
    p1 = new_snake(1, 4, new_snake(1, 3, new_snake(1, 2, new_snake(1, 1, NULL))));

    kaching *k = malloc(sizeof(kaching));   // prepare ka-ching
    k->x = 2;
    k->y = 2;

    int isAlive = 1;    // variable that indicate if the player is alive

    display(grid, p1, k);

    while(isAlive) {
        char command;       // the command entered by user
        int isInput = 0;    // bool to check if input

        int newX = p1->x;   // new coor, will be modified by user input
        int newY = p1->y;

        while (isInput == 0) {
            scanf("%s", &command);   // enter the command
            if (command == 'z') {       // 'z' to go up
                newX = p1->x-1;
            }
            else if (command == 's') {   // 's' to go down
                newX = p1->x+1;
            }
            else if (command == 'q') {   // 'q' to go left
                newY = p1->y-1;
            }
            else if (command == 'd') {   // 'd' to go right
                newY = p1->y+1;
            }
            isInput=1;
            fflush(stdin);
        }

        
        snake *lastBeforeMove;  // variable to get the tail

        if (grid[newX][newY] != '#') {   // verify if new coor is correct
            lastBeforeMove = get_tail(p1);   // get the tail of the snake (for the grow)
            move_snake(newX, newY, p1);
        }

        if (p1->x == k->x && p1->y == k->y) {   // if snake on ka-ching
            grow_snake(lastBeforeMove->x, lastBeforeMove->y, p1);
        }

        display(grid, p1, k);   // display the game
        
    }

    return 0;
}