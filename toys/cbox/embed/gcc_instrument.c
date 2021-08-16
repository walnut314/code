#include <stdio.h>

// https://gcc.gnu.org/ml/gcc/2004-12/msg00648.html
// gcc -finstrument-functions my_code.c
//
// coverage:
// http://bobah.net/d4d/tools/code-coverage-with-gcov

void __cyg_profile_func_enter (void *, void *) __attribute__((no_instrument_function));
void __cyg_profile_func_exit (void *, void *) __attribute__((no_instrument_function));

int depth = -1;

void __cyg_profile_func_enter (void *func,  void *caller)
{
    int n;

    depth++;
    for (n = 0; n < depth; n++)
        printf (" ");
    printf ("-> %p\n", func);
}

void __cyg_profile_func_exit (void *func, void *caller)
{
    int n;

    for (n = 0; n < depth; n++)
        printf (" ");
    printf ("<- %p\n", func);
    depth--;
}

void bar(void)
{
}

void foo (void)
{
    int x;
    for (x = 0; x < 4; x++)
        bar ();
}

int main (int argc, char *argv[])
{
    foo ();
    bar ();
    return 0;
}

