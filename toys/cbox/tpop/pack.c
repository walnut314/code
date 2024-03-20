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
    return pack(buf, "cscl", 'd', count, val, data);
}

int unpack_type0(int n, uchar *buf)
{
    uchar c1, c2;
    ushort count;
    ulong dw;
    if (unpack(buf, "cscl", &c1, &count, &c2, &dw) != n)
        return -1;
    printf("c1: %c, count: %d, c2: %c, data: %lx\n", c1, count, c2, dw);
    return n;
}

int pack_type1(uchar *buf, ushort count, ulong dw1, ulong dw2)
{
    return pack(buf, "csll", 'e', count, dw1, dw2);
}

int unpack_type1(int n, uchar *buf)
{
    uchar c;
    ushort count;
    ulong dw1, dw2;
    if (unpack(buf, "csll", &c, &count, &dw1, &dw2) != n)
        return -1;
    printf("c1: %c, count: %d, dw1: %c, dw2: %lx\n", c, count, dw1, dw2);
    return n;
}

int readpacket(int network, uchar *buf, size_t bufsize)
{
    switch (network) {
        case 0:
            return pack_type0(buf, 2, 4, 1234);
        case 1:
            return pack_type1(buf, 4, 1234, 5678);
        default:
            return 0;
    }
}

int (*unpackfn[])(int, uchar *) = {
    unpack_type0,
    unpack_type1,
    unpack_type2,
    unpack_type3};

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
        if ((*unpackfn[type](n, buf,) < 0)) {
            printf("protocol error, type 0x%x, length %d\n", type, n);
            return;
        }
                
    }
}

int main()
{
    uchar buf[256];
    int ret;
    memset(buf, 0, 256);
    ret = pack_type0(buf, 0, 'a', 0xdeadbeef);
    printf("ret: %d\n", ret);

    ret = unpack_type0(8, buf);
    printf("ret: %d\n", ret);
}

