#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef unsigned long   ulong;

#define NELEMS(arr) (sizeof(arr) / sizeof(arr[0]))

int pack(uchar *buf, char *fmt, ...)
{
    va_list args;
    char *p;
    uchar *bp;
    ushort s;
    ulong l;

    bp = buf;
    va_start(args, fmt);
    for (p = fmt; *p != '\0'; p++) {
        switch (*p) {
            case 'c':
                *bp++ = va_arg(args, int);
                break;
            case 's':
                s = va_arg(args, int);
                *bp++ = s >> 8;
                *bp++ = s;
                break;
            case 'l':
                l = va_arg(args, ulong);
                *bp++ = l >> 24;
                *bp++ = l >> 16;
                *bp++ = l >> 8;
                *bp++ = l;
                break;
            default:
                va_end(args);
                return -1;
        }
    }
    va_end(args);
    return bp - buf;
}

int unpack(uchar *buf, char *fmt, ...)
{
    va_list args;
    char *p;
    uchar *bp, *pc;
    ushort *ps;
    ulong *pl;

    bp = buf;
    va_start(args, fmt);
    for (p = fmt; *p != '\0'; p++) {
        switch (*p) {
            case 'c':
                pc = va_arg(args, uchar*);
                *pc = *bp++;
                break;
            case 's':
                ps = va_arg(args, ushort*);
                *ps = *bp++ << 8;
                *ps |= *bp++;
                break;
            case'l':
                pl = va_arg(args, ulong*);
                *pl  = *bp++ << 24;
                *pl |= *bp++ << 16;
                *pl |= *bp++ << 8;
                *pl |= *bp++;
                break;
            default:
                va_end(args);
                return -1;
        }
    }
    va_end(args);
    return bp - buf;
}

int pack_type0(uchar *buf, ushort count, uchar val, ulong data)
{
    return pack(buf, "cscl", 0x0, count, val, data);
}

int unpack_type0(int n, uchar *buf)
{
    uchar c1, c2;
    ushort count;
    ulong dw;
    static int counter = 0;
    counter++;
    if (unpack(buf, "cscl", &c1, &count, &c2, &dw) != n)
        return -1;
    printf("type0 c1: %d, count: %d, c2: %c, data: %lx\n", c1, count, c2, dw);
    if (counter < 4) {
        return n;
    } else {
        return -1;
    }
}

int pack_type1(uchar *buf, ushort count, ulong dw1, ulong dw2)
{
    return pack(buf, "csll", 0x1, count, dw1, dw2);
}

int unpack_type1(int n, uchar *buf)
{
    uchar c;
    ushort count;
    ulong dw1, dw2;
    static int counter = 0;
    counter++;
    if (unpack(buf, "csll", &c, &count, &dw1, &dw2) != n)
        return -1;
    printf("type1 c1: %d, count: %d, dw1: %lx, dw2: %lx\n", c, count, dw1, dw2);
    if (counter < 4) {
        return n;
    } else {
        return -1;
    }
}

int readpacket(int network, uchar *buf, size_t bufsize)
{
    switch (network) {
        case 0:
            return pack_type0(buf, 2, 'e', 0x1234);
        case 1:
            return pack_type1(buf, 4, 0x5678, 0x9abc);
        default:
            return 0;
    }
}

int (*unpackfn[])(int, uchar *) = {
    unpack_type0,
    unpack_type1};

void receive(int network)
{
    uchar type, buf[BUFSIZ];
    int n;

    while ((n = readpacket(network, buf, BUFSIZ)) > 0) {
        type = buf[0];
        if (type >= NELEMS(unpackfn)) {
            printf("bad packet type 0x%x\n", type);
            return;
        }
        if ((unpackfn[type](n, buf) < 0)) {
            printf("protocol error, type 0x%x, length %d\n", type, n);
            return;
        }
                
    }
}

int main()
{
    receive(0);
    receive(1);
}

