#include <stdio.h>
#include <string.h>

// convert file ascii from upper to lower case:
//    tr A-Z a-z < dict.txt > dict2.txt

int charcomp(char *x, char *y) { return *x - *y; }

#define WORDMAX     (100)

void scan()
{
    char word[WORDMAX], sig[WORDMAX];
    printf("scan\n");

    while (scanf("%s", word) != EOF) {
        strcpy(sig, word);
        qsort(sig, strlen(sig), sizeof(char), charcomp);
        printf("%s %s\n", sig, word);
    }
}

void squash()
{
    char word[WORDMAX], sig[WORDMAX], oldsig[WORDMAX];
    int linenum = 0;
    printf("squash\n");
    strcpy(oldsig, "");
    while (scanf("%s %s", sig, word) != EOF) {
        if (strcmp(oldsig, sig) != 0 && linenum > 0) 
            printf("\n");
        strcpy(oldsig, sig);
        linenum++;
        printf("%s ", word);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    if (strcmp(argv[1], "scan") == 0) {
        scan();
    } else
    if (strcmp(argv[1], "squash") == 0) {
        squash();
    }

}
