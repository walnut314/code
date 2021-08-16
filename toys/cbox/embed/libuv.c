#include <stdio.h>
#include <stdlib.h>
#include <uv.h>

/*
 * https://nikhilm.github.io/uvbook/
 *  gcc libuv.c /usr/local/lib/libuv.a
 */

int main() {
    uv_loop_t *loop = malloc(sizeof(uv_loop_t));
    uv_loop_init(loop);

    printf("Now quitting.\n");
    uv_run(loop, UV_RUN_DEFAULT);

    uv_loop_close(loop);
    free(loop);
    return 0;
}

