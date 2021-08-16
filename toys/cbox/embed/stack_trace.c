#include <stdio.h>
#define _STACK_INIT
#include "stack.inc"

int f1()
{
    ENTER();
    EXIT();    
}

int f2()
{
    ENTER();
    f1();
    EXIT();    
}

int f3()
{
    ENTER();
    f2();
    f2();
    f2();
    EXIT();    
}

int f4()
{
    ENTER();
    f3();
    EXIT();    
}

int main()
{
    f4();
    SHOW();
}
