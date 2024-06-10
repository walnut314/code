#ifndef _HARNESS_H_
#define _HARNESS_H_

#include <stdbool.h>

typedef struct _HARNESS {
    int in;
    int out;
    int expect;
    bool pass;
} HARNESS;



#endif // _HARNESS_H_

