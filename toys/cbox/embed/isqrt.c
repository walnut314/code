#include <stdio.h>
#include <math.h>

// from John Carmack Quake 3

float Q_rsqrt( float number )
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;                       // evil floating point bit level hacking
    printf("hexf 1: %0lx\n", i);
    i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
    printf("hexf 2: %0lx\n", i);
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//  y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

    return y;
}

int main()
{
    float x, y, z;
    x = 4;
    y = sqrt(x);
    z = Q_rsqrt(x);
    printf("inv sqrt(%f) = %f\n", x, z);
}
