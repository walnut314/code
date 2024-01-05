#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Nameval {
    char *name;
    int  value;
} Nameval;

struct NVtab {
    int nval;
    int max;
    Nameval *nameval;
} nvtab;

enum { NVINIT = 1, NVGROW = 2 };


int delname(char *name)
{
    return 0;
}

int addname(Nameval newname)
{
    Nameval *nvp;
    if (nvtab.nameval == NULL) {
        // malloc
        printf("init array\n");
        nvtab.nameval = (Nameval *) malloc(NVINIT * sizeof(Nameval));
        nvtab.max = NVINIT;
        nvtab.nval = 0;
    } else if (nvtab.nval >= nvtab.max) {
        // realloc
        printf("grow array\n");
        nvp = (Nameval *) realloc(nvtab.nameval, (NVGROW*nvtab.max) * sizeof(Nameval));
        nvtab.max *= NVGROW;
        nvtab.nameval = nvp;
    }
    nvtab.nameval[nvtab.nval] = newname;
    printf("adding[%d]: %s, %d\n", nvtab.nval, 
            nvtab.nameval[nvtab.nval].name,
            nvtab.nameval[nvtab.nval].value);
    return nvtab.nval++;
}

void show()
{
    for (int i = 0; i < nvtab.nval; i++) {
        printf("nvtab[%d] = %s : %d\n",
                i, nvtab.nameval[i].name, 
                nvtab.nameval[i].value);
    }
}

int main()
{
    nvtab.nameval = NULL;
    
    char *nms[] = { "fred", "wilma", "barney", "betty" };

    Nameval *nval;
    for (int i = 0; i < 4; i++) {
        nval = (Nameval *) malloc(sizeof(Nameval));
        nval->name = nms[i]; //strdup("fred");
        nval->value = i;
        addname(*nval);
    }
    show();
}
