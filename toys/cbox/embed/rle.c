#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void hexdump(const unsigned char *buffer, unsigned long long length)
{
    char OffsetBuf[10];
    unsigned long long LastStart = 0, i, k, j;
    unsigned long long elf_bias = -62;
    printf("hex: %d\n", length);
    for (i = 0 ; i < length ; i++) {
        if (i % 16 == 0) {
            sprintf(OffsetBuf, "%08x", i+elf_bias);
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

int rle_encode(char *out, const char *in, int l)
{
    int dl, i;
    char cp, c;
    for(cp=c=*in++, i=0, dl=0; l > 0; c = *in++, l--) {
        if (c == cp) {
            i++;
            if (i > 255) {
                *out++ = 255;
                *out++ = c; dl += 2;
                i = 1;
            }
        } else {
            *out++ = i;
            *out++ = cp; dl += 2;
            i = 1;
        }
        cp = c;
    }
    *out++ = i; *out++ = cp; dl += 2;
    return dl;
}

int rle_decode(char *out, const char *in, int l)
{
    int i, j, tb;
    char c;
 
    for(tb = 0; l > 0; l -= 2) {
        i = *in++;
        c = *in++;
        tb += i;
        while(i-- > 0) *out++ = c;
    }
    return tb;
}

const char *o = "WWWWWWWWWWWWBWWWWWWWWWWWWBBBWWWWWWWWWWWWWWWWWWWWWWWWBWWWWWWWWWWWWWW";
 
int main()
{
    char *encbuf = malloc(2*strlen(o));
    char *decbuf = malloc(strlen(o));

    printf("dump buf: ");
    printf("%s\n", o);
    //hexdump(o, strlen(o));
 
    int rl = rle_encode(encbuf, o, strlen(o));
    printf("dump enc buf\n");
    hexdump(encbuf, rl);
    //fwrite(encbuf, 1, rl, stdout);
 
    int ocl = rle_decode(decbuf, encbuf, rl);
    //fwrite(decbuf, 1, ocl, stdout);
    printf("%s\n", decbuf);
    //hexdump(decbuf, ocl);
 
    free(encbuf); free(decbuf);
    return 0;
}
