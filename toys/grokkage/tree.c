#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "fifo.h"
#include "ring.h"
#include "tree.h"


/*
 * good AVL code: https://www.zentut.com/c-tutorial/c-avl-tree/
 */

#if 0
typedef struct _tree {
    struct _tree *left, *right, *parent;
    struct _tree *sibling;
    int key;
    int height;
} tree;
#endif

#define dprint printf
//#define dprint(...)
#define MAX(a,b) (((a)>(b))?(a):(b))

#define SIZE_RING 32
#define SIZE_STAK 32
typedef tree* e_v;
#include "ring.inc"
#include "stak.inc"

tree *new(int key) {
    tree *t = (tree *) malloc(sizeof(tree));
    t->left = t->right = NULL;
    t->key = key;
    t->height = 0;
    //dprint("creating new node..%d\n", t->key);
    return t;
}

int theight(tree *root)
{
   if(root == NULL) return 0;
   return 1+MAX(theight(root->left), theight(root->right));
}

tree *max_node(tree *root) 
{
    tree *t = root;
    if (t == NULL) return NULL;
    while (t->right) t = t->right;
    dprint("max node..%d\n", t->key);
    return t;
}

tree *min_node(tree *root) 
{
    tree *t = root;
    if (t == NULL) return NULL;
    while (t->left) t = t->left;
    dprint("min node..%d\n", t->key);
    return t;
}

tree *avl_single_left(tree *root)
/*
 *      d           c
 *     /           / \
 *    c     -->   a   d
 *   / \             /
 *  a   b           b
 */
{
    tree *new_root;
    new_root = root->left;
    if (NULL == new_root) return root;
    root->left = new_root->right;
    new_root->right = root;
    root->height = MAX(theight(root->left), theight(root->right)) + 1;
    new_root->height = MAX(theight(new_root->left), root->height) + 1;
    return new_root;
}

tree *avl_single_right(tree *root)
/*
 *    c           a
 *   / \           \
 *  a   d   <--     c
 *   \             / \
 *    b           b   d
 */
{
    tree *new_root;
    new_root = root->right;
    if (NULL == new_root) return root;
    root->right = new_root->left;
    new_root->left = root;
    root->height = MAX(theight(root->left), theight(root->right)) + 1;
    new_root->height = MAX(theight(new_root->right), root->height) + 1;
    return new_root;
}

tree *avl_double_left(tree *root)
/*
 *         c          c -->         b
 *        /          /             / \
 *       a -->      b             a   c
 *        \        /
 *         b      a
 */
{
    root->left = avl_single_right( root->left );
    return avl_single_left( root );
}

tree *avl_double_right(tree *root)
/*
 *     a          a -->            b
 *      \          \              / \
 *       c -->      b            a   c
 *      /            \
 *     b              c
 */
{
    root->right = avl_single_left( root->right );
    return avl_single_right( root );
}

tree *insert_nr(tree *root, int key)
{
    tree *t = root;
    tree *tmp = NULL;
    tree *parent = NULL;
    tree *stack[32];
    int stack_ptr = 0;
    if (t == NULL) {
        t = new(key);
        t->parent = NULL;
        return t;
    } else
    tmp = t;
    parent = t;
    while (tmp) {
        parent = tmp;
        stack[stack_ptr++] = parent;
        if (key < tmp->key) {
            tmp = tmp->left;
        } else if (key > tmp->key) {
            tmp = tmp->right;
        }
    }
    printf("at insert point: %d\n", parent->key);
    if (key < parent->key) {
        parent->left = new(key);
        parent->left->parent = parent;
        stack[stack_ptr++] = parent->left;
        //printf("left height diff: %d\n", theight(root->left) - theight(root->right));
    } else if (key > parent->key) {
        parent->right = new(key);
        parent->right->parent = parent;
        stack[stack_ptr++] = parent->right;
        //printf("right height diff: %d\n", theight(root->left) - theight(root->right));
    }
    int hgt = theight(parent->left) - theight(parent->right);
    //if( hgt == 2 ) {
        printf("out of balance: %d\n", hgt);
    //}
    while (stack_ptr) {
        tree *st = stack[--stack_ptr];
        hgt = abs(theight(st->left) - theight(st->right));
        printf("hgt: %d\n", hgt);
        if (key < st->key) {
            st = avl_single_left(st);
        } else {
            //st = avl_double_left(st);
        }
#if 0
        if (st->parent == NULL) continue;
        if (st->parent->left && st->parent->right) {
            int hgt = theight(st->left) - theight(st->right);
            printf("height diff: %d\n", hgt);
        }
        if (st->parent->left) {
            int hgt = theight(st->parent->left);
            printf("left height: %d\n", hgt);
            if (hgt == 2) {
                //st->right = avl_single_right(st->left);
            }
        }
        if (st->parent->right) {
            int hgt = theight(st->parent->right);
            printf("left height: %d\n", hgt);
            if (hgt == 2) {
                //st->left = avl_single_left(st->right);
            }
        }
#endif
    }
    return t;
}

tree *insert_bal_rec(tree *root, int key)
{
    tree *t = root;
    if (t == NULL) {
        t = new(key);
        return t;
    } else
    if (key < t->key) {
        t->left = insert_bal_rec(t->left, key);
        if( theight(t->left) - theight(t->right) == 2 ) {
            dprint("left: out of balance\n");
            if (key < t->left->key) {
                dprint("avl_single_left (LL): new key %d, left->key %d\n", key, t->left->key);
                t = avl_single_left(t);
            } else {
                dprint("avl_double_left (LR): new key %d, left->key %d\n", key, t->left->key);
                t = avl_double_left(t);
            }
        }
    } else if (key > t->key) {
        t->right = insert_bal_rec(t->right, key);
        if( theight(t->right) - theight(t->left) == 2 ) {
            dprint("right: out of balance\n");
            if (key < t->right->key) {
                dprint("avl_single_right (RR): new key %d, right->key %d\n", key, t->right->key);
                t = avl_single_right(t);
            } else {
                dprint("avl_double_right (RL): new key %d, right->key %d\n", key, t->right->key);
                t = avl_double_right(t);
            }
        }
    }
    t->height = MAX( theight( t->left ), theight( t->right ) ) + 1;
    return t;
}

tree *insert(tree *root, int key)
{
    tree *t = root;
    if (t == NULL) {
        t = new(key);
        return t;
    } else
    if (key < t->key) {
        t->left  = insert(t->left, key);
    } else if (key > t->key) {
        t->right = insert(t->right, key);
    }
    t->height = MAX( theight( t->left ), theight( t->right ) ) + 1;
    return t;
}

tree *delete(tree *root, int key)
{
    tree *t = root;
    if (t == NULL) return t;
    if      (key < t->key) t->left = delete(t->left, key);
    else if (key > t->key) t->right = delete(t->right, key);
    else {
        tree *tmp = t;
        if (t->right && t->left) {
            //tree *min = min_node(t->right);
            //t->key = min->key;
            //t->right = delete(t->right, t->key);

            tree *max = max_node(t->left);
            t->key = max->key;
            t->left = delete(t->left, t->key);
        } else {
            if      (t->right == NULL) t = t->left;
            else if (t->left == NULL)  t = t->right;
            free(tmp);
        }
    }
    return t;
}

tree *delete_bal(tree *root, int key)
{
    tree *t = root;
    if (t == NULL) return t;
    if (key < t->key) {
        t->left = delete_bal(t->left, key);
        if( theight(t->right) - theight(t->left) > 2 ) {
            dprint("left: out of balance\n");
            if (key < t->left->key) {
                dprint("avl_single_left (LL): new key %d, left->key %d\n", key, t->left->key);
                t = avl_single_left(t);
            } else {
                dprint("avl_double_left (LR): new key %d, left->key %d\n", key, t->left->key);
                t = avl_double_left(t);
            }
        }
    } else if (key > t->key) {
        t->right = delete_bal(t->right, key);
        if( theight(t->left) - theight(t->right) >  2 ) {
            dprint("right: out of balance\n");
            if (key < t->right->key) {
                dprint("avl_single_right (RR): new key %d, right->key %d\n", key, t->right->key);
                t = avl_single_right(t);
            } else {
                dprint("avl_double_right (RL): new key %d, right->key %d\n", key, t->right->key);
                t = avl_double_right(t);
            }
        }
    } else {
        tree *tmp = t;
        if (t->right && t->left) {
            //tree *min = min_node(t->right);
            //t->key = min->key;
            //t->right = delete_bal(t->right, t->key);
            tree *max = max_node(t->left);
            t->key = max->key;
            t->left = delete_bal(t->left, t->key);
        } else {
            if      (t->right == NULL) t = t->left;
            else if (t->left == NULL)  t = t->right;
            free(tmp);
        }
    }
    return t;
}


int deletemin(tree **pT)
{
    int min;
    if ((*pT)->left == NULL) {
        min = (*pT)->key;
        (*pT) = (*pT)->right;
        return min;
    } else {
        return deletemin(&((*pT)->left));
    }
}

tree *tfind(tree *root, int key)
{
    if (root == NULL) return NULL;
    if      (root->key < key) tfind(root->right, key);
    else if (root->key > key) tfind(root->left, key);
    else {
        printf("found it %d\n", root->key);
        return root;
    }
    return NULL;
}

void bfs(tree *root) 
{
    tree *n = root, *p;
    printf("bfs - enter\n");
    init_ring();
    rngput(n);
    while (!rngempty()) {
        n = rngget();
        printf("tree: %d\n", n->key);
        if (n->left)  rngput(n->left);
        if (n->right) rngput(n->right);
    }
}

#if 0
void bfs2(tree *root) 
{
    tree *n = root, *p;
    ring_ptr r1 = ring_create(64);   
    printf("bfs2(ring) - enter\n");
    ring_push(r1, (void *) n);
    while (!ring_empty(r1)) {
        n = ring_shift(r1);
        printf("tree: %d\n", n->key);
        if (n->left)  ring_push(r1, (void *) n->left);
        if (n->right) ring_push(r1, (void *) n->right);
    }
}
void bfs3(tree *root) 
{
    tree *n = root;
    fifo_ptr f = fifo_new(); 
    printf("bfs3(fifo) - enter\n");
    fifo_put(f, (void *) n);
    while (!fifo_empty(f)) {
        n = fifo_get(f);
        printf("tree: %d\n", n->key);
        if (n->left)  fifo_put(f, (void *) n->left);
        if (n->right) fifo_put(f, (void *) n->right);
    }
}
#endif

/* stack-based pre-order traversal */
void dfs(tree *root) 
{
    tree *n = root, *p;
    printf("dfs - enter\n");
    init_stak();
    stkpush(n);
    while (!stkempty()) {
        n = stkpop();
        printf("tree: %d\n", n->key);
        if (n->right) stkpush(n->right);
        if (n->left)  stkpush(n->left);
    }
    printf("dfs - exit\n");
}

void pre_order(tree *root)
{
    if (root) {
        printf("root->key: %d\n", root->key);
        //printf("%c ", root->key);
        pre_order(root->left);
        pre_order(root->right);
    }
}

void in_order(tree *root)
{
    if (root) {
        in_order(root->left);
        printf("root->key: %d\n", root->key);
        //printf("%c ", root->key);
        in_order(root->right);
    }
}

void post_order(tree *root)
{
    if (root) {
        post_order(root->left);
        post_order(root->right);
        printf("root->key: %d\n", root->key);
        //printf("%c ", root->key);
    }
}

#if 0
#define MAX_DEPTH 16
int _print_t(tree *tree, int is_left, int offset, int depth, char s[MAX_DEPTH][255])
{
    char b[MAX_DEPTH];
    int width = 5;
    int left, right, i;
    if (!tree) return 0;
    sprintf(b, "(%03d)", tree->key);
    //sprintf(b, "(%c)", tree->key);
    left  = _print_t(tree->left,  1, offset,                depth + 1, s);
    right = _print_t(tree->right, 0, offset + left + width, depth + 1, s);
    for (i = 0; i < width; i++)
        s[depth][offset + left + i] = b[i];
    if (depth && is_left) {
        for (i = 0; i < width + right; i++)
            s[depth - 1][offset + left + width/2 + i] = '-';
        s[depth - 1][offset + left + width/2] = '.';
    } else if (depth && !is_left) {
        for (i = 0; i < left + width; i++)
            s[depth - 1][offset - width/2 + i] = '-';
        s[depth - 1][offset + left + width/2] = '.';
    }
    return left + width + right;
}

int print_t(tree *tree)
{
    char s[MAX_DEPTH][255];
    int i;
    for (i = 0; i < MAX_DEPTH; i++)
        sprintf(s[i], "%80s", " ");
    _print_t(tree, 0, 0, 0, s);
    for (i = 0; i < MAX_DEPTH; i++)
        printf("%s\n", s[i]);
    return 0;
}
#endif

void manual_build()
{
    tree *root = NULL, *t;
    root = new(8);
    root->left = new(7);
    root->right = new(9);
    bfs(root);
    dfs(root);
    print_t(root);
    printf("height = %d\n", theight(root));
}

void siblings(tree *root) 
{
    if (NULL == root) return;
    if (root->left) root->left->sibling = root->right;
    if (root->right) root->right->sibling = (root->sibling) ? root->sibling->left : NULL;
    siblings(root->left);
    siblings(root->right);
}
#if 0
#define END_OF_LEVEL (NULL)
void sib2(tree *root)
{
    tree *p, *n;
    fifo_ptr f = fifo_new();
    printf("sib2 - enter\n");
    fifo_put(f, (void *) root);
    fifo_put(f, (void *) END_OF_LEVEL); // mark end of level
    while (!fifo_empty(f)) {
        p = fifo_get(f);
        while (p != END_OF_LEVEL) {
            printf("bfs: %d\n", p->key);
            // create/walk the sibling list, nodes at same level
            p->sibling = fifo_get(f);
            if (p->left) fifo_put(f, p->left);
            if (p->right) fifo_put(f, p->right);
            p = p->sibling;
        } 
        // all nodes at this level are visited - mark DONE
        if (!fifo_empty(f)) fifo_put(f, END_OF_LEVEL);
    }
}
#endif
void sib1(tree *root)
{
    root->sibling = NULL;
    siblings(root);    
}

void sibtrav(tree *root)
{
    tree *p;
    if (root == NULL) return;
    sibtrav(root->left);
    p = root->sibling;
    printf("p: %d, ", root->key);
    while (p) {
        printf("%d, ", p->key);
        p = p->sibling;
    }
    printf("\n");
    sibtrav(root->right);
}

int is_keyid(char c)
{
    if ((c >= '0' && c <= '9') ||
        (c == '+' || c == '*')) {
        //printf("%c is keyid\n", c);
        return 1;
    }
    //printf("%c is not keyid\n", c);
    return 0;
}

char my_itoa(int i)
{
    char a = (i + '0');
    return a;
}

void post_fix() // RPN
{
    tree *x, *z;
    char c;
    z = (tree *) malloc(sizeof(tree));
    z->left = z->right = z;
    z->key = 0;
    init_stak();
    // only works for single digit operands
    for ( ; scanf("%c", &c) != EOF; ) {
        //printf("scanning: %c\n", c);
        if (!is_keyid(c)) continue;
        x = (tree *) malloc(sizeof(tree));
        x->key = c; x->left = x->right = z;
        if (c == '+' || c == '*') {
            x->right = stkpop(); x->left = stkpop();
            int a = atoi((const char *) &x->right->key);
            int b = atoi((const char *) &x->left->key);
            if (c == '*') {
                x->key = my_itoa(a * b);
                printf("mul: %d + %d = %d\n", a, b, a * b);
                stkpush(x); // ???
            }
            if (c == '+') {
                x->key = my_itoa(a + b);
                printf("add: %d + %d = %d\n", a, b, a + b);
                stkpush(x); // ???
            }
        } else {
            //printf("pushing: %d\n", atoi((const char *) &x->key));
            stkpush(x);
        }
    }
    while (!stkempty()) {
        x = stkpop();
        printf("final stack keyues: %d\n", atoi((const char *) &x->key));
    }
}

void pre_fix()
{
    tree *root = NULL, *t;
    root = new('*');
    t = new('A'); root->left = t;
    t = new('+'); root->right = t;

    t = new('F'); root->right->right = t;
    t = new('*'); root->right->left = t;
    t = new('+'); root->right->left->left = t;
    t = new('B'); root->right->left->left->left = t;
    t = new('C'); root->right->left->left->right = t;
    t = new('*'); root->right->left->right = t;
    t = new('D'); root->right->left->right->left = t;
    t = new('E'); root->right->left->right->right = t;

    printf("algebraic: ");
    in_order(root);
    printf("\n");
    printf("      pre: ");
    pre_order(root);
    printf("\n");
    printf("      rpn: ");
    post_order(root);
    printf("\n");
}

int main()
{
    //manual_build();
    //return 0;

    tree *root = NULL, *t;
    //char a[] = {13, 3, 4, 12, 14, 10, 5, 1, 8, 2, 7, 9, 11, 6, 18};
    //int a[] = {2,1,3,42,5,4,17};
    //int a[] = {10, 5, 4, 3, 4, 7, 16, 13, 11, 13, 20, 18, 17, 16, 17, 19, 30};
    int a[] = {1,2,3,4,5,6,7,8}; //,9,10};
    //char a[] = {'T', 'A', 'M', 'E', 'E', 'A', 'P', 'S', 'L', 'E', 'R'};
    int i, sz = sizeof(a) / sizeof(a[0]);
    for (i = 0; i < sz; ++i) {
        //root = insert(root, a[i]);
        //root = insert_nr(root, a[i]);
        root = insert_bal_rec(root, a[i]);
    }
    print_t(root);
    return 0;
    //printf("pre order\n");
    //pre_order(root);
    //printf("in order\n");
    //in_order(root);
    //printf("post order\n");
    //post_order(root);
    //printf("height = %d\n", theight(root));
    //bfs2(root);
    //bfs3(root);
    //sib1(root);
    //sib2(root);
    //sibtrav(root);
    //bfs(root);
    dfs(root);
    //post_fix();
    //pre_fix();
    //return 0;
    //print_t(root);
    //printf("height = %d\n", theight(root));
    //printf("\n");
    for (i = 0; i < sz; ++i) {
        t = tfind(root, a[i]);
        if (t) printf("found (%d)\n", t->key);
    }
    for (i = 0; i < sz; ++i) {
        printf("delete: %d\n", a[i]);
        root = delete_bal(root, a[i]);
        printf("show..\n");
        print_t(root);
        printf("\n");
    }
    return 0;
}

