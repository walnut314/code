#include <stdio.h>
#include <string.h>

// Hex Dump In Many Programming Languages
// http://c2.com/cgi/wiki?HexDumpInManyProgrammingLanguages
//

void hexdump(const unsigned char *buffer, unsigned long long length)
{
    char OffsetBuf[10];
    unsigned long long LastStart = 0, i, k, j;
    unsigned long long elf_bias = -62;
    for (i = 0 ; i < length ; i++) {
        if (i % 16 == 0) {
            sprintf(OffsetBuf, "%08llx", i);
            printf("%4s:", OffsetBuf);
        }
        if (i % 4 == 0) {
            printf(" ");
        }
        printf("%02x ", buffer[i]);
        if (i % 16 == 15 || i == length-1) {
            if (i == length-1) {
                for (k = i % 16 ; k < 15 ; k++) {
                     printf( "   " );
                     if (k % 4 == 3 && k % 16 != 0) {
                         printf(" ");
                     }
                }
            }
            printf("|");
            for (j = LastStart ; j <= i ; j++) {
                if (j % 4 == 0) {
                    printf(" ");
                }
                if (buffer[j] > 31 && buffer[j] < 127) {
                    printf("%c", buffer[j]);
                } else {
                    printf(".");
                }
            }
            if (i == length-1) {
                printf("\n");
                break;
            }
            printf("\n");
            LastStart += 16;
        }        
    }
}

int BUFFER_SIZE = 2000000;
#define FILENAME_SIZE  (32)
FILE *hfile;
int n;
int count = 0;
int written = 0;

int main(int argc, char *argv[])
{
    char buffer[BUFFER_SIZE];
    char source[FILENAME_SIZE];
    if (argc == 2) {
        strcpy(source, argv[1]);
    } else {
        printf("usage: binhex <file>\n");
        return -1;
    }

    hfile = fopen(source, "rb");

    if (hfile) {
        printf("binhex %s\n", source);
        while (!feof(hfile)) {
            n = fread(buffer, 1, BUFFER_SIZE, hfile);
            count += n;
            printf("n = %d\n", n);
            hexdump((const unsigned char *) buffer, n);
        }
        printf("%d bytes read from library.\n", count);
    } else {
        printf("fail\n");
    }
    fclose(hfile);

    return 0;
}


