#include <stdio.h>
#include <stdlib.h>

typedef struct _LIST {
    struct _LIST *next;
    int item;
} LIST, *PLIST;

PLIST reverse(LIST *head)
{
    PLIST p = head->next, q, r = NULL;
    while (p) {
        q = p->next;
        p->next = r;
        r = p;
        p = q;
    }
    return r;
}

void add(LIST *head, int item)
{
    PLIST entry = (PLIST) malloc(sizeof(LIST));
    PLIST p;
    printf("add: %d\n", item);
    entry->item = item;
    p = head->next;
    head->next = entry;
    entry->next = p;
}

void delete(LIST *head, int item)
{
    PLIST p, n;
    n = head->next;
    p = head;
    while (n) {
        if (n->item == item) {
            printf("delete: %d\n", item);
            p->next = n->next;
            return;
        }
        p = n;
        n = n->next;
    }
}

void dump(PLIST head)
{
    PLIST p = head->next;
    while (p) {
        printf("P: %d\n", p->item);
        p = p->next;
    }
}

int main()
{
    LIST head, *rev;
    head.next = NULL;
    for (int i = 0; i < 8; i++) {
        add(&head, i);
    }
    dump(&head);
    rev->next = reverse(&head);
    delete(rev, 0);
    delete(rev, 4);
    delete(rev, 7);
    dump(rev);
}

