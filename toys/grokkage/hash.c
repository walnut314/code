#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <math.h>
#include "hash.h"

enum { MULTIPLIER = 31, NBUCKETS = 255 };

typedef struct _map_entry {
    struct _map_entry *next;
    char  *key;
    int value;
} map_entry;

typedef struct map {
    unsigned int buckets;
    unsigned int multiplier;
    map_entry **prtab;
} map;

map_ptr map_new() {
    map_entry **pr;
    map_ptr hash;
    int i;
    hash = (map_ptr) malloc(sizeof(*hash));
    pr = (map_entry **) malloc(NBUCKETS * sizeof(map_entry*));
    hash->prtab = pr;
    hash->buckets = NBUCKETS;
    hash->multiplier = MULTIPLIER;
    for (i = 0; i < (int) NBUCKETS; i++) {
        hash->prtab[i] = (map_entry *) malloc(sizeof(map_entry));
        hash->prtab[i]->next = NULL;
    }
    return hash;
}

void map_delete(map_ptr tab) {
    map_entry **prtab = tab->prtab, *pr, *pr_prev, *tmp;
    int i;
    for (i = 0; i < tab->buckets; ++i) {
        pr = prtab[i]->next;
        pr_prev = prtab[i];
        while (pr) {
            tmp = pr;
            printf("free %s -> %d\n", pr->key, pr->value);
            free(pr->key);
            pr_prev->next = pr->next;
            pr_prev = pr;
            pr = pr->next;
            free(tmp);
        }
        free(prtab[i]);
    }
    free(prtab);
    free(tab);
}

// this is knuths algorithm for integers, can be used for pointers as well
unsigned int integer_hash(unsigned long long key, unsigned int buckets)
{
//#define MULT (2654435761)
#define MULT (0x9E3779B1)
    return (((key * MULT) % (unsigned int) pow(2,32)) % buckets);
}

unsigned int string_hash(char *str, unsigned int buckets, unsigned int multiplier)
{
    unsigned char *p;
    unsigned int h = 0;
    for (p = (unsigned char *) str; *p != '\0'; p++) {
        h = (multiplier * h) + *p;
    }
    return (h % buckets);
}

bool map_lookup(map_ptr tab, char *key, int *value)
{
    int h;
    map_entry *pr, **prtab;
    prtab = tab->prtab;
    h = string_hash(key, tab->buckets, tab->multiplier);
    for (pr = prtab[h]->next; pr != NULL; pr = pr->next) {
        if (strcmp(key, pr->key) == 0) {
            //printf("found %d -> %s\n", pr->value, pr->key);
            *value = pr->value;
            return true;

        }
    }
    return false;
}

bool map_insert(map_ptr tab, char *key, int value)
{
    int h;
    char *dup_key;
    map_entry *pr, **prtab;
    prtab = tab->prtab;
    h = string_hash(key, tab->buckets, tab->multiplier);
    pr = (map_entry *) malloc(sizeof(map_entry));
    dup_key = strdup(key);
    pr->key = dup_key;
    pr->value = value;
    pr->next = prtab[h]->next;
    prtab[h]->next = pr;
    printf("map_inserted(%s, %d)\n", pr->key, pr->value);
    return true;
}

bool map_remove(map_ptr tab, char *key, int *value)
{
    int h;
    map_entry *pr, *prev_pr, **prtab, *tmp;
    prtab = tab->prtab;
    h = string_hash(key, tab->buckets, tab->multiplier);
    prev_pr = prtab[h];
    pr = prtab[h]->next;
    while (pr != NULL) {
        if (strcmp(key, pr->key) == 0) {
            printf("removing %d -> %s\n", pr->value, pr->key);
            tmp = pr;
            *value = pr->value;
            prev_pr->next = pr->next;
            free(tmp);
            return true;
        }
        prev_pr = pr;
        pr = pr->next;
    }
    return false;
}

void iterate(map_ptr tab) {
    int i, j;
    map_entry **prtab = tab->prtab;
    printf("hash iterate...(enter)\n");
    for (i = 0; i < tab->buckets; ++i) {
        map_entry *pr = prtab[i]->next;
        j = 0;
        while (pr) {
            printf("tab[%d][%d] = %s -> %d\n", i, j, pr->key, pr->value);
            pr = pr->next;
            ++j;
        }
    }
    printf("hash iterate...(exit)\n");
}

#if 1
int main()
{
    map_entry *pr;
    char key[8];
    map *tab;
    int value;
    tab = map_new();

    map_insert(tab, "dude", 76);
    map_insert(tab, "dude", 76);
    map_insert(tab, "bart", 77);
    map_insert(tab, "cindy", 78);
    map_insert(tab, "zoey", 79);
    map_insert(tab, "rowdy", 80);

    strcpy(key, "dude");
    if (map_lookup(tab, key, &value)) {
        printf("found: %s -> %d\n", key, value);
    } else {
        printf("not found: %s\n", key);
    }

    iterate(tab);

    if (map_remove(tab, key, &value)) {
        printf("removed %s -> %d\n", key, value);
        free(pr);
    }
    if (map_lookup(tab, key, &value)) {
        printf("found: %s -> %d\n", key, value);
    } else {
        printf("not found: %s\n", key);
    }
    map_delete(tab);
    return 0;
}
#endif

