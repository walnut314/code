#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _List {
    struct _List *next;
    char *value;
    int key;
    void *ctx;
} List;

int list_empty(List *head)
{
    return head->next == NULL ? 1 : 0;
}

void list_dump(List *head)
{
    if (!head) return;
    List *n = head->next;
    while (n) {
        printf("%d, ", n->key);
        n = n->next;
    }
    printf("\n");
}

void list_dump2(List *head)
{
    if (!head) return;
    List *n = head->next;
    while (!list_empty(n)) {
        printf("%d, ", n->key);
        n = n->next;
    }
    printf("\n");
}

List *list_create(int key, char *value, void *ctx)
{
    List *n = (List *) malloc(sizeof(List));
    n->next = NULL;
    n->key = key;
    n->value = _strdup(value);
    n->ctx = ctx;
    return n;
}

void list_prepend(List *head, int key, char *value, void *ctx)
{
    List *n = list_create(key, value, ctx);
    List *tmp = head->next;
    head->next = n;
    n->next = tmp;
}

void list_append(List *head, int key, char *value, void *ctx)
{
    List *n = list_create(key, value, ctx);
    List *tmp = head->next;
    List *prev = head;
    while (tmp) {
        prev = tmp;
        tmp = tmp->next;
    }
    prev->next = n;
}

List *list_find(List *head, int key, char *value)
{
    if (!head) return NULL;
    List *n = head->next;
    while (n) {
        if (n->key == key && !strcmp(n->value, value)) {
            return n;
        }
        n = n->next;
    }
    return NULL;
}

typedef struct _Tree {
    struct _Tree *left;
    struct _Tree *right;
    int key;
} Tree, *pTree;

void list_test()
{
    List *head = list_create(-1, NULL, NULL);
    for (int i = 0; i < 4; i++) {
        list_append(head, i, NULL, NULL);
        //list_prepend(head, i, NULL);
    }
    list_prepend(head, 42, NULL, NULL);
    list_append(head, 47, NULL, NULL);
    list_dump2(head);
}

Tree *tree_create(int key)
{
    Tree *n = (Tree *) malloc(sizeof(Tree));
    n->left = n->right = NULL;
    n->key = key;
    return n;
}

void tree_free(Tree *root)
{
    root->left = root->right = NULL;
    root->key = -1;
    free(root);
}

Tree *tree_insert(Tree *root, int key)
{
    if (root == NULL)         root        = tree_create(key);
    else if (key < root->key) root->left  = tree_insert(root->left, key);
    else if (key > root->key) root->right = tree_insert(root->right, key);
    else printf("dup: %d\n", key);
    return root;
}

int tree_delete_min(Tree **root)
{
    int min;
    if ((*root)->left == NULL) {
        min = (*root)->key;
        Tree *tmp = *root;
        *root = (*root)->right;
        tree_free(tmp);
        return min;
    } else {
        return tree_delete_min(&(*root)->left);
    }
}

Tree *tree_find(Tree *root, int key)
{
    if (root == NULL)         return root;
    else if (key == root->key) return root;
    else if (key < root->key) tree_find(root->left, key);
    else if (key > root->key) tree_find(root->right, key);
}

void tree_bfs(Tree *root)
{
    List *queue = list_create(0, NULL, root);
    list_append(queue, 0, NULL, root);
    printf("BFS\n");
    while (!list_empty(queue)) {
        List *node = queue->next;
        queue->next = node->next;
        Tree *t = node->ctx;
        printf("t:%d\n", t->key);
        if (t->left) list_append(queue, 0, NULL, t->left);
        if (t->right) list_append(queue, 0, NULL, t->right);
    }
}

void tree_dump(Tree *root)
{
    if (root == NULL) return;
    tree_dump(root->left);
    printf("%d\n", root->key);
    tree_dump(root->right);
}

void tree_test()
{
    Tree *root = NULL;
    for (int i = 0; i < 4; i++) {
        root = tree_insert(root, i);
    }
    tree_dump(root);
    tree_bfs(root);
#if 0
    Tree *find;
    int key = 42;
    if (find = tree_find(root, key)) printf("found: %d\n", find->key);
    else printf("not found: %d\n", key);
#endif
    printf("min: %d\n", tree_delete_min(&root));
    printf("min: %d\n", tree_delete_min(&root));
    printf("min: %d\n", tree_delete_min(&root));
}

#define HASH_SZ (101)
List *table[HASH_SZ];
int hash(char *str)
{
    int hash = 5381;
    int c;
    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % HASH_SZ;
}

void hash_init()
{
    for (int i = 0; i < HASH_SZ; i++) {
        table[i] = list_create(-1, NULL, NULL);
    }
}

void hash_insert(char *key, int value)
{
    int hval = hash(key);
    list_append(table[hval], value, key, NULL);
}

int hash_find(char *key, int value)
{
    int hval = hash(key);
    List *f = list_find(table[hval], value, key);
    if (f) {
        printf("found: %d\n", f->key);
    } else {
        printf("not found %d\n", value);
    }
    return 0;
}

void hash_dump()
{
    for (int i = 0; i < HASH_SZ; i++) {
        printf("tbl[%03d]: ", i);
        list_dump(table[i]);
    }
}

void hash_test()
{
    hash_init();
    hash_insert("dude", 42);
    hash_insert("dude", 43);
    hash_insert("homi", 43);
    hash_insert("sup", 44);
    hash_insert("dawg", 45);
    hash_find("dude", 42);
    hash_find("dawg", 45);
    //hash_dump();
}

int main()
{
    //list_test();
    //tree_test();
    //hash_test();
}

