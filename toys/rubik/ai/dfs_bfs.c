#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_NODES   (10)
#define MAX_STACK   (10)
#define MAX_QUEUE   (10)

int adjMatrix[NUM_NODES][NUM_NODES];
int visited[NUM_NODES];

int stack[MAX_STACK];
int stack_index;

int queue[MAX_QUEUE];
int write, read;
#define WRAP(x)     ((x == (MAX_QUEUE)-1) ? 0 : x+1)

void initAdjMatrix()
{
    memset((void *) adjMatrix, 0, sizeof(adjMatrix));
    memset((void *) visited, 0, sizeof(visited));
}

void makeEdge(int from, int to)
{
    adjMatrix[from][to] = 1;
}

void initStack()
{
    stack_index = 0;
}

void push(int elem)
{
    stack[stack_index++] = elem;
}

int pop()
{
    int elem;
    elem = stack[--stack_index];
    return elem;
}

int empty()
{
    if (stack_index == 0) return 1;
    else return 0;
}

int initQueue()
{
    write = read = 0;
}

int Qempty()
{
    if (write == read) return 1;
    else return 0;
}

void enqueue(int elem)
{
    queue[write] = elem;
    write = WRAP(write);
}

int dequeue()
{
    int elem;
    elem = queue[read];
    read = WRAP(read);
    return elem;
}

void dfs(int start, int goal)
{
    int node, i;
    push(start);
    while (!empty()) {
        node = pop();
        if (node == goal) {
            printf("%d Goal\n", node);
            return;
        }

        if (visited[node]) continue;
        else visited[node] = 1;

        printf("%d ", node);
        /* push children on stack */
        for (i = NUM_NODES-1; i >= 0; i--) {
            if (adjMatrix[node][i] == 1) push(i);
        }
    }
    printf("Goal not found\n");
}

void bfs(int start, int goal)
{
    int node, i;
    enqueue(start);
    while (!Qempty()) {
        node = dequeue();
        if (node == goal) {
            printf("%d Goal\n", node);
            return;
        }
        if (visited[node]) continue;
        else visited[node] = 1;
        printf("%d ", node);
        /* enqueue all children */
        for (i = 0; i < NUM_NODES; i++) {
            if (adjMatrix[node][i] == 1) enqueue(i);
        }
    }
    printf("Goal not found\n");
}

int main()
{
    initAdjMatrix();
    initStack();
    initQueue();

    makeEdge(1, 2);
    makeEdge(1, 3);
    makeEdge(1, 4);
    makeEdge(2, 5);
    makeEdge(3, 5);
    makeEdge(3, 6);
    makeEdge(4, 7);
    makeEdge(5, 8);
    makeEdge(5, 9);

    //dfs(1, 6);
    bfs(1, 9);
}

