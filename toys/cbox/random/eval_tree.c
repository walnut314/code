#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct node {
   char op;
   int val;
   struct node *left;
   struct node *right;
};

typedef struct node Tree;

int generate(int codep, Tree *t);

enum {
    NUMBER,
    VARIABLE,
    ADD,
    SUBTRACT,
    DIVIDE,
    MULTIPLY,
    MAX,
    ASSIGN,
    ENUM_SZ
};

typedef struct Symbol Symbol;
struct Symbol {
    int value;
    char *name;
};

typedef union Code Code;
union Code {
    void (*op)(void);
    int value;
    Symbol *symbol;
};


#define NCODE   (32)
#define NSTACK  (32)
Code code[NCODE];
int stack[NSTACK];
int pc = 0;
int codep = 0;
int stackp = 0;


void pushop() { 
    printf("pushop\n");
    //int op = stack[stackp];
    //stack[++stackp] = op;
}

void pushsymop() {
    printf("pushsymop\n");
}

void addop() { 
    printf("addop\n");
    //int left, right;
    //right = stack[--stackp];
    //left = stack[--stackp];
    //stack[stackp++] = left + right;
}

void subop() { 
    printf("subop\n");
    //int left, right;
    //right = stack[--stackp];
    //left = stack[--stackp];
    //stack[stackp++] = left - right;
}

void multop() { 
    printf("multop\n");
    //int left, right;
    //right = stack[--stackp];
    //left = stack[--stackp];
    //stack[stackp++] = left * right;
}

void divop() { 
    printf("divop\n");
    //int left, right;
    //right = stack[--stackp];
    //left = stack[--stackp];
    //if (right == 0) {
    //    printf("divide by zero: %d\n", left);
    //}
    //stack[stackp++] = left / right;
}

void maxop() { 
    printf("maxop\n");
}

void asnop() {
    printf("asnop\n");
}


char *get_token(char **p){
    while(**p && isspace(**p)){//skip spaces
        ++*p;
    }
    char *ret = *p;
    if(!*ret){//empty
        return NULL;
    }
    while(**p && !isspace(**p)){
        ++*p;
    }
    if(!**p){
        **p = 0;
        ++*p;
    }
    return ret;
}

Tree *make_tree(char **p){
    char *token = get_token(p);
    printf("token: %s\n", token);

    if(token){
        Tree *tree = malloc(sizeof(*tree));

        switch(*token){//token[1] == '\0' ??
        case '*':
        case '/':
        case '+':
        case '-':
            tree->op = *token;
            tree->val = 0;
            tree->left = make_tree(p);
            tree->right = make_tree(p);
            break;
        default:
            tree->op = 'N';
            tree->val = atoi(token);
            tree->left = tree->right = NULL;
        }
        return tree;
    }
    return NULL;
}

void release(Tree *tree){
    if(tree){
        release(tree->left);
        release(tree->right);
        free(tree);
    }
}

void print_infix(Tree *tree){
    if(tree){
        switch(tree->op){
        case '*':
        case '/':
        case '+':
        case '-':
            putchar('(');
            print_infix(tree->left);
            printf(" %c ", tree->op);
            print_infix(tree->right);
            putchar(')');
            break;
        case 'N':
            printf("%d", tree->val);
            break;
        default:
            fprintf(stderr, "\nerror!\n");//this should never be executed
        }
    }
}

void dump_code()
{
    int i;
    for (i = 0; i < codep; i++) {
        printf("code[%d] = %p\n", i, code[i].op);
    }
}

int eval(Tree *t)
{
    int c = 0;
    codep = generate(0, t);
    printf("codep: %d\n", codep);
    code[codep].op = NULL;
    stackp = 0;
    codep = 0;
    
    while (code[codep].op != NULL) {
        printf("codep: %d %p\n", 
                codep,
                code[codep].op);
        codep++;
        //(*code[codep++].op)();
    }
    return stack[0];
}

int generate(int codep, Tree *t)
{
    printf("generate: %d op %c\n", codep, t->op);
    switch (t->op) {
        case NUMBER:
        case 'N':
            code[codep++].op = pushop;
            code[codep++].value = t->val;
            return codep;
        case VARIABLE:
            code[codep++].op = pushsymop;
            code[codep++].value = t->val;
            return codep;
        case ADD:
        case '+':
            codep = generate(codep, t->left);
            codep = generate(codep, t->right);
            code[codep++].op = addop;
            return codep;
        case SUBTRACT:
        case '-':
            codep = generate(codep, t->left);
            codep = generate(codep, t->right);
            code[codep++].op = subop;
            return codep;
        case MULTIPLY:
        case '*':
            codep = generate(codep, t->left);
            codep = generate(codep, t->right);
            code[codep++].op = multop;
            return codep;
        case DIVIDE:
        case '/':
            codep = generate(codep, t->left);
            codep = generate(codep, t->right);
            code[codep++].op = divop;
            return codep;
        case MAX:
        case ASSIGN:
            break;
    }
    return codep; //next op
}



void multi_line()
{
    char line[256];

    while(fgets(line, sizeof(line), stdin)){
        printf("line..\n");
        char *s = line;
        Tree *tree = make_tree(&s);
        print_infix(tree);
        putchar('\n');
        release(tree);
    }
}

void one_liner()
{
    char line[256] = "* + 7 3 - 5 2";
    char *s = line;
    Tree *tree = make_tree(&s);
    print_infix(tree);
    putchar('\n');
    eval(tree);
    //codep = generate(codep, tree);
    //printf("codep=%d\n", codep);
    //dump_code();
    release(tree);
}

int main(void){
    //multi_line();
    one_liner();
    return 0;
}
#if 0
* + 7 3 - 5 2
((7 + 3) * (5 - 2))
* 3 + 1  2
(3 * (1 + 2))
/ + 1 + 2 3 5
((1 + (2 + 3)) / 5)
^Z
#endif
