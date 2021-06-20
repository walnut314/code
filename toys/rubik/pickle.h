#include <stdio.h>
#include <stdlib.h>

void pickle(const char *file_name, void *data, int len)
{
    FILE *f;
    f = fopen(file_name,"wb");
    if (!f) {
        printf("Unable to open file!");
        return;
    }
    fwrite(data, len, 1, f);    
    fclose(f);
};

void unpickle(const char *file_name, void *data, int len)
{
    FILE *f;
    f = fopen(file_name,"rb");
    if (!f) {
        printf("Unable to open file!");
        return;
    }
    fread(data, len, 1, f);    
    fclose(f);
}


