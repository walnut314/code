#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define PROFILE_PRINT 3

struct Profile {
    uint32_t count;
    uint32_t sum;
    uint32_t start;
    uint32_t end;
};

int main()
{
    struct Profile prof;

    prof.count = 0;
    prof.sum = 0;
    while (1) {

        prof.start = time(NULL);

        // do something
        sleep(1);
        //

        prof.end = time(NULL);

        prof.sum += (prof.end - prof.start);
        prof.count++;

        if (prof.count == PROFILE_PRINT) {
            printf("profile: %d\n", prof.sum);
            prof.count = 0;
            prof.sum = 0;
        }

    }

}
