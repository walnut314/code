#ifndef _UTIL_H_
#define _UTIL_H_


#include <assert.h>
#include <stdio.h>
#include <stdbool.h>


typedef char *string;
string String(char *);

typedef struct U_boolList_ *U_boolList;

struct U_boolList_ {bool head; U_boolList tail;};

U_boolList U_BoolList(bool head, U_boolList tail);

void *checked_malloc(int);

#endif // _UTIL_H_
