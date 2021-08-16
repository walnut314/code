#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct timer_list
{
    struct timer_list *next, *prev;
    uint32_t expires;
    uint32_t data;
    void (*callback)(uint32_t, uint32_t);
};

struct timer_list master;

void timer_handler(uint32_t exp, uint32_t data)
{
}

void init_timer(struct timer_list *tlist)
{
    printf("init_timer\n");
}

void add_timer(struct timer_list *tlist)
{
    struct timer_list *e = (struct timer_list *) malloc(sizeof(struct timer_list));
    struct timer_list *m = &master;
    printf("add_timer\n");
    e->expires = tlist->expires;
    e->data = tlist->data;
    e->next = m->next;
    m->next = e;

}

void enum_timers()
{
    struct timer_list *m = &master;
    struct timer_list *n = m->next;
    while (n) {
        printf("entery: %p\n", m);
        n = n->next;
    }
}

int main()
{
    struct timer_list test_timer;
    uint32_t time;
    uint32_t data;

    master.next = NULL;

    init_timer(&test_timer);
    add_timer(&test_timer);
    enum_timers();
    
}

