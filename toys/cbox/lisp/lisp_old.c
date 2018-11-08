#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpc.h"

// linux
// gcc -std=c99 -Wall lisp.c mpc.c -ledit -lreadline -o lisp
// windows
// gcc -std=c99 -Wall lisp.c mpc.c -o lisp

/* If we are compiling on Windows compile these functions */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/* Fake readline function */
char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer)+1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy)-1] = '\0';
    return cpy;
}

/* Fake add_history function */
void add_history(char* unused) {
}

/* Otherwise include the editline headers */
#else
#include <editline/readline.h>
//#include <editline/history.h>
#endif

typedef struct lval {
    int type;
    long num;

    char *err;
    char *sym;

    int count;
    struct lval** cell;

} lval;

enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR };
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

lval *lval_num(long x)
{
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

// prototypes
lval *lval_take(lval *v, int i);
lval *lval_eval(lval *v);
lval *lval_pop(lval *v, int i);
lval *builtin_list(lval *a);
lval *builtin_eval(lval *a);
lval *builtin_head(lval *a);
lval *builtin_tail(lval *a);
lval *builtin_join(lval *a);

lval *lval_err(char *m)
{
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    v->err = malloc(strlen(m)+1);
    strcpy(v->err, m);
    return v;
}

lval *lval_sym(char *s)
{
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s)+1);
    strcpy(v->sym, s);
    return v;
}

lval *lval_sexpr(void)
{
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

lval *lval_qexpr(void)
{
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_QEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

void lval_del(lval *v)
{
    switch (v->type) {
        case LVAL_NUM: break;
        case LVAL_ERR: free(v->err); break;
        case LVAL_SYM: free(v->sym); break;

        case LVAL_QEXPR: 
        case LVAL_SEXPR: 
            for (int i = 0; i < v->count; i++) {
                lval_del(v->cell[i]);
            }
            free(v->cell);
            break;
    }
    free(v);
}

lval *lval_read_num(mpc_ast_t *t)
{
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ?
        lval_num(x) : lval_err("invalid number");
}

lval *lval_add(lval *v, lval *x)
{
    v->count++;
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    v->cell[v->count-1] = x;
    return v;
}

lval *lval_join(lval *x, lval *y)
{
    while (y->count) {
        x = lval_add(x, lval_pop(y, 0));
    }

    // delete empty y and return x
    lval_del(y);
    return x;
}

lval *lval_read(mpc_ast_t *t)
{
    if (strstr(t->tag, "number")) { return lval_read_num(t); }
    if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }

    // if root (>) or sexpr then create empty list
    lval *x = NULL;
    if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); }
    if (strstr(t->tag, "sexpr"))  { x = lval_sexpr(); }
    if (strstr(t->tag, "qexpr"))  { x = lval_qexpr(); }

    // fill this list with any valid expressions
    for (int i = 0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
        if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
        if (strcmp(t->children[i]->tag,  "regex") == 0) { continue; }
        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
}


void lval_expr_print(lval *v, char open, char close);

void lval_print(lval *v)
{
    switch (v->type) {
        case LVAL_NUM: printf("%li", v->num); break;
        case LVAL_ERR: printf("Error: %s", v->err); break;
        case LVAL_SYM: printf("%s", v->sym); break;
        case LVAL_SEXPR: lval_expr_print(v, '(', ')'); break;
        case LVAL_QEXPR: lval_expr_print(v, '{', '}'); break;
    }
}

void lval_expr_print(lval *v, char open, char close)
{
    putchar(open);
    for (int i = 0; i < v->count; i++) {
        // print value contained within
        lval_print(v->cell[i]);

        // don't print trailing space if last element
        if (i != (v->count-1)) {
            putchar(' ');
        }
    }
    putchar(close);

}

void lval_println(lval *v) { lval_print(v); putchar('\n'); }

lval *lval_pop(lval *v, int i)
{
    // find the item at "i"
    lval *x = v->cell[i];

    // shift memory after the item at i over the top
    memmove(&v->cell[i], &v->cell[i+1], sizeof(lval*) * (v->count-i-1));

    // decrease the item count
    v->count--;
    // reallocate the memory used
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);

    return x;
}

// macros
#define LASSERT(args, cond, err) \
    if (!(cond)) { lval_del(args); return lval_err(err); }

lval *builtin_op(lval *a, char *op)
{
    // ensure all args are numbers
    for (int i = 0; i < a->count; i++) {
        if (a->cell[i]->type != LVAL_NUM) {
            lval_del(a);
            return lval_err("cannot operate on non-number");
        }
    }

    // pop first element
    lval *x = lval_pop(a, 0);

    // if no args and sub then perform unary negation
    if ((strcmp(op, "-") == 0) && a->count == 0) {
        x->num = -x->num;
    }

    // while there are still remaining elements
    while (a->count > 0) {
        // pop next element
        lval *y = lval_pop(a, 0);

        if (strcmp(op, "+") == 0) { x->num += y->num; }
        if (strcmp(op, "-") == 0) { x->num -= y->num; }
        if (strcmp(op, "*") == 0) { x->num *= y->num; }
        if (strcmp(op, "/") == 0) {
            if (y->num == 0) {
                lval_del(x); lval_del(y);
                x = lval_err("divide by zero"); break;
            }
            x->num /= y->num;
        }
        lval_del(y);
    }
    lval_del(a);
    return x;
}

lval *builtin(lval *a, char *func)
{
    if (strcmp("list", func) == 0) { return builtin_list(a); }
    if (strcmp("head", func) == 0) { return builtin_head(a); }
    if (strcmp("tail", func) == 0) { return builtin_tail(a); }
    if (strcmp("join", func) == 0) { return builtin_join(a); }
    if (strcmp("eval", func) == 0) { return builtin_eval(a); }
    if (strstr("+-*/", func))      { return builtin_op(a, func); }
    lval_del(a);
    return lval_err("unknown function");
}

lval *builtin_head(lval *a)
{
    LASSERT(a, a->count == 1, "head pass too many args");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "head incorrect type");
    LASSERT(a, a->cell[0]->count != 0, "head passed {}");

    lval *v = lval_take(a, 0);
    while (v->count > 1) { lval_del(lval_pop(v, 1)); }

    return v;

}

lval *builtin_tail(lval *a)
{
    LASSERT(a, a->count == 1, "tail pass too many args");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "tail incorrect type");
    LASSERT(a, a->cell[0]->count != 0, "tail passed {}");

    lval *v = lval_take(a, 0);
    lval_del(lval_pop(v, 0));

    return v;

}

lval *builtin_list(lval *a)
{
    a->type = LVAL_QEXPR;
    return a;
}

lval *builtin_eval(lval *a)
{
    LASSERT(a, a->count == 1, "eval pass too many args");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "eval incorrect type");

    lval *x = lval_take(a, 0);
    x->type = LVAL_SEXPR;

    return lval_eval(x);
}

lval *builtin_join(lval *a)
{
    for (int i = 0; i < a->count; i++) {
        LASSERT(a, a->cell[i]->type == LVAL_QEXPR, "join incorrect type");
    }

    lval *x = lval_pop(a, 0);

    while (a->count) {
        x = lval_join(x, lval_pop(a, 0));
    }

    lval_del(a);
    return x;
}

lval *lval_eval_sexpr(lval *v); // fwd decl

lval *lval_take(lval *v, int i)
{
    lval *x = lval_pop(v, i);
    lval_del(v);
    return x;
}

lval *lval_eval(lval *v)
{
    // evaluate s-expressions
    if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(v); }
    // all other types remain the same
    return v;
}

lval *lval_eval_sexpr(lval *v)
{
    // evaluate children
    for (int i = 0; i < v->count; i++) {
        v->cell[i] = lval_eval(v->cell[i]);
    }

    // error checking
    for (int i = 0; i < v->count; i++) {
        if (v->cell[i]->type == LVAL_ERR) { return lval_take(v, i); }
    }

    // empty expressions
    if (v->count == 0) { return v; }

    // single expression
    if (v->count == 1) { return lval_take(v, 0); }

    // ensure first element is a symbol
    lval *f = lval_pop(v, 0);
    if (f->type != LVAL_SYM) {
        lval_del(f); lval_del(v);
        return lval_err("S-expression does not start with symbol");
    }

    // call built-in with operator
    lval *result = builtin(v, f->sym);
    lval_del(f);
    return result;

}

void usage()
{
    printf("usage: \n");
}

int main(int argc, char** argv) {

    puts("Lisp Version 0.0.0.0.1");
    puts("Press Ctrl+c to Exit\n");

    mpc_parser_t* Number    = mpc_new("number");
    mpc_parser_t* Symbol    = mpc_new("symbol");
    mpc_parser_t* Sexpr     = mpc_new("sexpr");
    mpc_parser_t* Qexpr     = mpc_new("qexpr");
    mpc_parser_t* Expr      = mpc_new("expr");
    mpc_parser_t* Lispy     = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                           \
            number      : /-?[0-9]+/ ;                              \
            symbol      : \"list\" | \"head\" | \"tail\"            \
                        | \"join\" | \"eval\"                       \
                        | '+' | '-' | '*' | '/' ;                   \
            sexpr       : '(' <expr>* ')' ;                         \
            qexpr       : '{' <expr>* '}' ;                         \
            expr        : <number> | <symbol> | <sexpr> | <qexpr> ; \
            lispy       : /^/ <expr>* /$/ ;                         \
        ",
        Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

    while (1) {

        /* Now in either case readline will be correctly defined */
        char* input = readline("lisp> ");

        add_history(input);

        //printf("%s\n", input);
        if (strcmp(input, "help") == 0) {
            usage();
            continue;
        }

        if (strcmp(input, "quit") == 0) {
            free(input);
            break;
        }

        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {

            lval *x = lval_eval(lval_read(r.output));
            lval_println(x);
            lval_del(x);
            //mpc_ast_print(r.output);

            mpc_ast_delete(r.output);
        } else {
            // on error print the error
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);

    }

    mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

    return 0;
}
