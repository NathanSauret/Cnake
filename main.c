#include <stdio.h>
#include <stdlib.h>


int SIZE = 10;


typedef struct snake snake;
struct snake {
    int x;
    int y;
    snake *next;
};


snake* ajouter_liste(int x, int y, snake *p)
// add a snake
{
    snake *el;

    el = malloc(sizeof(snake));
    el->x = x;
    el->y = y;
    el->next = p;
    return el;
}



int main()
{
    
    return 0;
}