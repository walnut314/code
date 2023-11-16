#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define NUM_NODES   (10)
#define MAX_STACK   (10)

int adjMatrix[NUM_NODES][NUM_NODES];
int visited[NUM_NODES];

int stack[MAX_STACK];
int stack_index;

void initAdjMatrix()
{
    memset((void *) adjMatrix, 0, sizeof(adjMatrix));
    memset((void *) visited,   0, sizeof(visited));
}

void makeEdge(int from, int to)
{
    assert(from < NUM_NODES);
    assert(to   < NUM_NODES);
    adjMatrix[from][to] = 1;
}

void intStack()
{
    stack_index = 0;
}

void push(int elem)
{
    assert(stack_index < MAX_STACK);
    stack[stack_index++] = elem;
}

int pop()
{
    int elem;
    assert(stack_index > 0);
    elem = stack[--stack_index];
    return elem;
}

int empty()
{
    if (stack_index == 0) return 1;
    else return 0;
}


int main()
{
}
