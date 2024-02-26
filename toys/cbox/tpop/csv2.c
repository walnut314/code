#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csv.h"

static void reset(void)
{
    free(line);
    free(sline);
    free(field);
    line = NULL;
    sline = NULL;
    field = NULL;
    maxline = maxfield = nfield = 0;
}

static int endofline(FILE *fin, int c)
{
    int eol;

    eol = (c == '\r' || c == '\n');
    if (c == '\r') {
        c = getc(fin);
        if (c != '\n' && c != EOF)
            ungetc(c, fin); // read too far, put c back
    }
    return eol;
}

static char *advquoted(char *p)
{
    int i, j;

    for (i = j = 0; p[j] != '\0'; i++, j++) {
        if (p[j] == '"' && p[++j] != '"') {
            // copy up to next separator or \0
            int k = strcspn(p+j, fieldsep);
            memmove(p+i, p+j, k);
            i += k;
            j += k;
            break;
        }
        p[i] = p[j];
    }
    p[i] = '\0';
    return p+j;
}

static int split(void)
{
    char *p, **newf;
    char *sepp;
    int sepc;

    nfield = 0;
    if (line[0] == '\0')
        return 0;
    strcpy(sline, line);
    p = sline;

    do {
        if (nfield >= maxfield) {
            maxfield *= 2;
            newf = (char **) realloc(field, maxfield * sizeof(field[0]));
            if (newf == NULL)
                return NOMEM;
            field = newf;
        }
        if (*p == '"')
            sepp = advquoted(++p); // skip initial quote
        else
            sepp = p + strcspn(p, fieldsep);
        sepc = sepp[0];
        sepp[0] = '\0';
        field[nfield++] = p;
        p = sepp + 1;
    } while (sepc == ',');
    
    return nfield;
}

char *csvgetline(FILE *fin)
{
    int i, c;
    char *newl, *news;

    if (line == NULL) {
        maxline = maxfield = 1;
        line = (char *) malloc(maxline);
        sline = (char *) malloc(maxline);
        field = (char **) malloc(maxfield*sizeof(field[0]));
        if (line == NULL || sline == NULL || field == NULL) {
            reset();
            return NULL;
        }
    }
    for (i = 0; (c = getc(fin)) != EOF && !endofline(fin, c); i++) {
        if (i >= maxline-1) { // grow line
            maxline *= 2;
            newl = (char *) realloc(line, maxline);
            news = (char *) realloc(sline, maxline);
            if (newl == NULL || news == NULL) {
                reset();
                return NULL;
            }
            line = newl;
            sline = news;
        }
        line[i] = c;
    }
    line[i] = '\0';
    if (split() == NOMEM) {
        reset();
        return NULL;
    }
    return (c == EOF && i == 0) ? NULL : line;
}

char *csvfield(int n)
{
    if (n < 0 || n >= nfield)
        return NULL;
    return field[n];
}

int csvnfield(void)
{
    return nfield;
}


int main()
{
    int i;
    char *line;

    while ((line = csvgetline(stdin)) != NULL) {
        printf("line = '%s'\n", line);
        for (i = 0; i < csvnfield(); i++) 
            printf("field[%d] = '%s'\n", i, csvfield(i));
    }
    return 0;
}

