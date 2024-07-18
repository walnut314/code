#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int bit_count(int x)
{
    int cnt = 0;
    while (x) {
        x &= (x-1);
        cnt++;
    }
    return cnt;
}

// https://github.com/ericwang14/Programming-Pearls-source-code 
int cmp(const void* a, const void* b) {
   return (*(int*)a - *(int*)b);
}

// shuffle n items and print m sorted from shuffle
void genshuf(int m, int n)
{
    int i, j;
    int *x = (int *) malloc(n * sizeof(int));
    for (i = 0; i < n; i++)
        x[i] = i;
    for (i = 0; i < m; i++) {
        j = (rand() % (n-1)) + i; // random (i, n-1)
        int t = x[i]; x[i] = x[j]; x[j] = t;
    }
    qsort(x, m, sizeof(int), cmp);
    for (i = 0; i < m; i++)
        printf("%d\n", x[i]);
}

void reverse(int a[], int i, int j)
{
    while (i < j) {
        // swap 'em
        int t = a[i]; a[i] = a[j]; a[j] = t;
        i++; j--;
    }
}

void show(int a[], int n)
{
    printf("==> ");
    for (int i = 0; i < n; i++) {
        printf("%c, ", a[i]);
    }
    printf("\n");
}

void shift_left(int a[], int i, int n)
{
    reverse(a, 0, i-1); show(a, n);
    reverse(a, i, n-1); show(a, n);
    reverse(a, 0, n-1); show(a, n);
}

typedef struct _List {
    struct _List *next;
    int item;
} List;

void ListShow(List *head)
{
    List *temp = head->next;
    while (temp) {
        printf("l: %d\n", temp->item);
        temp = temp->next; 
    }
}

void ListInsert(List *head, int item)
{
    List *temp = (List *) malloc(sizeof(List));
    temp->item = item;
    temp->next = head->next;
    head->next = temp;
}

int main()
{
    //int a[] = {'a','b','c','d','e','f','g','h'};
    int a[] = {'a','b','c'};
    int len = sizeof(a) / sizeof(a[0]);
    //shift_left(a, 3, len);

    show(a, len);
    reverse(a, 0, 2); // swap 0 and 2
    show(a, len);

    genshuf(10, 100);

    printf("bits: %d\n", bit_count(0x17));

    List head; head.next = NULL;
    for (int i = 0; i < 5; i++) {
        ListInsert(&head, i);
    }
    ListShow(&head);
}

