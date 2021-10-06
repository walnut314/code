#include <stdio.h>
#include <stdint.h>

struct dude {
    union {
        struct {
        uint8_t is_static : 1; // LSB
        uint8_t is_extern : 1;
        uint8_t is_auto   : 1;
        uint8_t is_naker  : 3;
        uint8_t is_done   : 2; // MSB
        };
        uint8_t as_byte;
    };
};

int main()
{
    struct dude d = {0};
    d.is_static = 1;
    d.is_done = 3;
    printf("%02xh\n", d.as_byte);
}

