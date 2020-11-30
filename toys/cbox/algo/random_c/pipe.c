#include <stdio.h>

int main(int argc, char** argv)
{
    char line[256];
    char *pline = line;
    ssize_t read;
    int nr_lines=0, nr_bytes=0;

    while (fgets(pline, sizeof line, stdin) != NULL) {
        printf("%d: line: %s", nr_lines, pline);
        nr_lines++;
        nr_bytes += read;
    }
    return 0;
}


