#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>

#define BUFSIZE (256)
#define MINLEN (6)

void strings(char *name, FILE *fin)
{
    int c, i;
    char buf[BUFSIZE];

    do {
        for (i = 0; (c = getc(fin)) != EOF; ) {
            if (!isprint(c))
                break;
            buf[i++] = c;
            if (i > BUFSIZ)
                break;
        }
        if (i >= MINLEN)
            printf("%s:%.*s\n", name, i, buf);
    } while (c != EOF);
}

int main(int argc, char *argv[])
{
    int i;
    FILE *fin;

    if (argc == 1)
        printf("usage: strings filenames");
    else {
        for (i = 1; i < argc; i++) {
            if ((fin = fopen(argv[i], "rb")) == NULL)
                printf("can't open %s:", argv[i]);
            else {
                printf("here: %s\n", argv[i]);
                strings(argv[i], fin);
                fclose(fin);
            }
        }
    }
    return 0;
}


