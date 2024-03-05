#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <math.h>

#include "eprintf.h"

static char *name = NULL;

void eprintf(char *fmt, ...)
{
    va_list args;

    fflush(stdout);
    if (progname() != NULL)
        fprintf(stderr, "%s: ", progname());

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    if (fmt[0] != '\0' && fmt[strlen(fmt)-1] == ':')
        fprintf(stderr, " %s", strerror(errno));
    fprintf(stderr, "\n");
    exit(2);
}

char *estrdup(char *s)
{
    char *t;

    t = (char *) malloc(strlen(s)+1);
    if (t == NULL)
        eprintf("estrdup(\"%.20s\") failed:", s);
    strcpy(t, s);
    return t;
}

void *emalloc(size_t n)
{
    void *p;

    p = malloc(n);
    if (p == NULL)
        eprintf("malloc of %u bytes failed:", n);
    return p;
}

void setprognam(char *str)
{
    name = estrdup(str);
}

char *progname(void)
{
    return name;
}

int main(int argc, char *argv[])
{
    FILE *f;
    double d;

    setprognam("markov");
    //f = fopen(argv[1], "r");
    //if (f == NULL) {
    //    eprintf("can't open %s: ", argv[1]);
    //}
    errno = 0;
    d = log(-1.23);
    printf("errno: %f %d %s\n", d, errno, strerror(errno));
}


