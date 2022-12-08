#include <stdio.h>
#include <stdlib.h>

#define FSM            for(;;)
#define STATE(x)       x##_s 
#define NEXTSTATE(x)   goto x##_s

int state(int k)
{
    printf("state - enter\n");
    FSM {
        STATE(s1):
          printf("state 1\n");
          NEXTSTATE(s3);

        STATE(s2):
          //... do stuff ...
          printf("state 2\n");
          break;
          //if (k<0) NEXTSTATE(s1); 
          /* fallthrough as the switch() cases */

        STATE(s3):
          printf("state 3\n");
          NEXTSTATE(s2);
          //... final stuff ...
          break;  /* Exit from the FSM */
    }
    printf("state - exit\n");
    return k;
}

int main()
{
    int s = 0;
    while (1) {
        state(s);
        char c = getchar();
        if (c == 'q') exit(1);
        s++;
        s %= 4;
    }
}
