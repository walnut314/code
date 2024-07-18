#include <stdio.h>
#include <stdlib.h>

typedef struct _list {
    struct _list *next;
    int val;
} list;

void list_show(list *l)
{
    printf("list show:\n");
    list *next = l;
    while (next) {
        printf("%d ", next->val);
        next = next->next;
    }
    printf("\n");
}

void list_add(list *head, int val)
{
    list *new = (list *) malloc(sizeof(list));
    new->val = val;
    new->next = head->next;
    head->next = new;
}

list *list_pop(list *head)
{
    list *t = head->next;
    head->next = t->next;
    return t;
}

list *list_reverse(list *head)
{
    list *p = head->next, *q, *r = NULL;
    while (p) {
        q = p->next;
        p->next = r;
        r = p;
        p = q;
    }
    return r;
}

int count_bits(int val)
{
    int count = 0;
    while (val) {
        count++;
        val &= (val-1);
    }
    return count;
}

void isort(int *a, int n)
{
    for (int c = 1; c <= n-1; c++) {
        int d = c;
        while (d > 0 && a[d] < a[d-1]) {
            //exchg(a[d], a[d-1]);
            int temp = a[d];
            a[d] = a[d-1];
            a[d-1] = temp;
            d--;
        }
    }
}

void show(int *a, int len)
{
    for (int i = 0; i < len; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    int val = 0x3013;
    printf("there are %d bits in %x\n", count_bits(val), val);

    int a[] = {3,2,5,1,9,6};
    int len = sizeof(a)/sizeof(a[0]);
    show(a, len);
    isort(a, len);
    show(a, len);

    list head;
    head.next = NULL;
    for (int i = 0; i < len; i++) {
        list_add(&head, a[i]);
    }
    list_show(head.next);
    printf("reverse\n");
    list *l = list_reverse(&head);
    list_show(l);
    head.next = l;
    while (head.next) {
        list *t = list_pop(&head);
        printf("%d ", t->val);
        free(t);
    }
    printf("\n");

}
