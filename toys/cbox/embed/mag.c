#include <stdio.h>
#include <math.h>

// Direction (y>0) = 90 - [arcTAN(x/y)]*180/¹
// Direction (y<0) = 270 - [arcTAN(x/y)]*180/¹
// Direction (y=0, x<0) = 180.0
// Direction (y=0, x>0) = 0.0

int main()
{
    float x, y, d, angle;
    //x = sqrt(2);
    //y = sqrt(2);
    x = -1.0;
    y = 0.0;

    angle = atan2(y, x) * (180.0/3.14);
    printf("angle: %f\n", angle);

    if (y > 0.0) {
        d = 90.0 - atan(x/y)*(180.0/3.14);
    } else
    if (y < 0.0) {
        d = 270.0 - atan(x/y)*(180.0/3.14);
    } else
    if (y == 0.0 && x < 0.0) {
        d = 180.0;
    } else
    if (y == 0.0 && x > 0.0) {
        d = 0.0;
    }

    printf("dir: %f\n", d);
}
