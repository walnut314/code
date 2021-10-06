#include <stdio.h>
#include <math.h>

//#include "MadgwickAHRS\MadgwickAHRS.h"
#include "MahonyAHRS\MahonyAHRS.h"

typedef struct _Quaternion {
    float w;
    float x;
    float y;
    float z;
} Quaternion;

static void toEulerAngle(const Quaternion q, double *roll, double *pitch, double *yaw);

int main()
{
    float gx, gy, gz, ax, ay, az, mx, my, mz;
    double roll, pitch, yaw; // eulers
    Quaternion q;

    // generate data
    gx = 0.0f;
    gy = 0.0f;
    gz = 0.0f;
    ax = 0.0f;
    ay = 0.0f;
    az = 0.0f;
    mx = 0.0f;
    my = 0.0f;
    mz = 0.0f;

    //
    MahonyAHRSupdate(gx, gy, gz, ax, ay, az, mx, my, mz);
    MahonyAHRSupdateIMU(gx, gy, gz, ax, ay, az);

    // convert the Madgwick quaternions to Euler angles
    q.w = q0; q.x = q1; q.y = q2; q.z = q3; // quaternions
    toEulerAngle(q, &roll, &pitch, &yaw);
    // write Madgwick results to CSV
    printf("%f, %f, %f\n", roll, pitch, yaw);
    
    //MadgwickAHRSupdate(gx, gy, gz, ax, ay, az, mx, my, mz);
    //MadgwickAHRSupdateIMU(gx, gy, gz, ax, ay, az);
    //
}

static void toEulerAngle(const Quaternion q, double *roll, double *pitch, double *yaw)
{
    // roll (x-axis rotation)
    double sinr_cosp = +2.0 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = +1.0 - 2.0 * (q.x * q.x + q.y * q.y);
    *roll = atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    double sinp = +2.0 * (q.w * q.y - q.z * q.x);
    if (fabs(sinp) >= 1)
        *pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
    else
        *pitch = asin(sinp);

    // yaw (z-axis rotation)
    double siny_cosp = +2.0 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = +1.0 - 2.0 * (q.y * q.y + q.z * q.z);  
    *yaw = atan2(siny_cosp, cosy_cosp);
}

