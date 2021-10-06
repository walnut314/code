#include <stdio.h>
#include <stdint.h>

typedef void (*funcptr)(void);
typedef int (*optr)(int);

void loader()
{
    printf("hello, loader\n");
}

int xopen(int h)  { printf("xopen:  %d\n", h); return 0; }
int xread(int h)  { printf("xread:  %d\n", h); return 0; }
int xwrite(int h) { printf("xwrite: %d\n", h); return 0; }
int xioctl(int h) { printf("xioctl: %d\n", h); return 0; }
int xseek(int h)  { printf("xseek:  %d\n", h); return 0; }
int xclose(int h) { printf("xclose: %d\n", h); return 0; }

struct fops {
    optr open;
    optr read;
    optr write;
    optr ioctl;
    optr seek;
    optr close;
};

int main()
{
    uint64_t ldr_start_addr = (uint64_t) &loader;
    funcptr jumper = (funcptr)(ldr_start_addr);
    printf("%p => %llx\n", jumper, ldr_start_addr);
    (*jumper)();

    struct fops ops = {
        .open  = xopen, 
        .read  = xread, 
        .write = xwrite, 
        .ioctl = xioctl, 
        .seek  = xseek, 
        .close = xclose
    };
    ops.open(42);
    ops.close(17);

    uint64_t ldr_op_addr;
    ldr_op_addr = (uint64_t) &xopen;  optr jopen  = (optr) ldr_op_addr;
    ldr_op_addr = (uint64_t) &xread;  optr jread  = (optr) ldr_op_addr;
    ldr_op_addr = (uint64_t) &xwrite; optr jwrite = (optr) ldr_op_addr;
    ldr_op_addr = (uint64_t) &xioctl; optr jioctl = (optr) ldr_op_addr;
    ldr_op_addr = (uint64_t) &xseek;  optr jseek  = (optr) ldr_op_addr;
    ldr_op_addr = (uint64_t) &xclose; optr jclose = (optr) ldr_op_addr;

    struct fops jumper2;
    jumper2.open    = jopen;
    jumper2.read    = jread;
    jumper2.write   = jwrite;
    jumper2.ioctl   = jioctl;
    jumper2.seek    = jseek;
    jumper2.close   = jclose;

    jumper2.open(43);
    jumper2.read(44);
    jumper2.write(45);
    jumper2.ioctl(46);
    jumper2.seek(47);
    jumper2.close(48);
    
}

