#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef unsigned long   ulong;


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

int pack_type1(uchar *buf, ushort count, uchar val, ulong data)
{
    return pack(buf, "cscl", 0x01, count, val, data);
}

int unpack_type1(int n, uchar *buf)
{
    uchar c1, c2;
    ushort count;
    ulong dw;
    if (unpack(buf, "cscl", &c1, &count, &c2, &dw) != n)
        return -1;
    printf("c1: %c, count: %d, c2: %c, data: %lx\n", c1, count, c2, dw);
    return n;
}

int main()
{
    uchar buf[256];
    int ret;
    memset(buf, 0, 256);
    ret = pack_type1(buf, 0x03, 'a', 0xdeadbeef);
    printf("ret: %d\n", ret);

    ret = unpack_type1(8, buf);
    printf("ret: %d\n", ret);
}

