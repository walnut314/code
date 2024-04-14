#include <math.h>
#include <stdio.h>


int main()
{
    double Time = 0.0;
    double Speed = 100.0;     // Speed rather than velocity, as it is only the magnitude
    double Angle = 0.52;      // Initial angle of 30º
    double xposition = 0.0;
    double yposition = 0.0;
    double xvelocity;
    double yvelocity;

    xvelocity = Speed * cos(Angle);
    yvelocity = Speed * sin(Angle);

    printf("sin %lf, cos %lf\b", sin(Angle), cos(Angle));

    for (int i = 0; i < 8; i++) {


        printf("vel(%lf, %lf)\n", xvelocity, yvelocity);
        printf("pos(%lf, %lf)\n", xposition, yposition);
        printf("\n");

        yvelocity = -9.8 * Time; // Apply gravity

        xposition += (xvelocity * Time);
        yposition += (yvelocity * Time);


        Time = Time + 0.5f;

    }
}

