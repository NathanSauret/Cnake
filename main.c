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


int isSnake(int x, int y, snake *pOG)
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


void display(char grid[SIZE][SIZE], snake *p, kaching *k)
// display the game
{
    for (int i=0; i<SIZE; i++) {
        for (int j=0; j<SIZE; j++) {

            if (isSnake(i, j, p)) {
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
}



int main()
{
    char grid[SIZE][SIZE];  // prepare the grid
    prepare_grid(grid);

    snake *p1;                  // prepare thr player
    p1 = new_snake(1, 1, NULL);

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

        if (0 < newX && newX < SIZE-1 && 0 < newY && newY < SIZE-1) {   // verify if new coor is correct
            p1->x = newX;
            p1->y = newY;
        }
        printf("%d | %d\n", newX, newY);

        display(grid, p1, k);
        
    }
    return 0;
}