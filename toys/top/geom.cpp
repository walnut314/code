#include "util.h"

// lats and longs are in degrees
// radius in km
// compute the geographical distance between two locations on a sphere’s surface
double greatCircleDistance(double pLat, double pLong,
                    double qLat, double qLong,
                    double radius)
{
    pLat *= PI/180.0; pLong *= PI/180.0;
    qLat *= PI/180.0; qLong *= PI/180.0;

    return radius * acos(
            cos(pLat)*cos(pLong)*cos(qLat)*cos(qLong) +
            cos(pLat)*sin(pLong)*cos(qLat)*sin(qLong) +
            sin(pLat)*sin(qLat)
            );
    return 0.0;
}

struct point { int x, y; };
// default to integer points
// the area of an n-sided polygon (either convex or concave) with 
// n pairs of vertex coordinates in some order (cw or ccw) is
// (1/2) * determinant(vertices)
int determinant(vector<point> P)
{
    int result = 0, x1, y1, x2, y2;
    for (int i = 0; i < P.size(); i++) {
        x1 = P[i].x;
        x2 = P[(i+1) % P.size()].x;
        y1 = P[i].y;
        y2 = P[(i+1) % P.size()].y;
        result += (x1 * y2 - x2 * y1);
    }
    return result;
}

// perimeter of n-sided polygon with n pairs of coords in some order
// (clockwise or counter-clockwise) computed with Pythagorean's theorem.
double perimeter(vector<point> P)
{
    double result = 0.0, x1, y1, x2, y2, dx, dy;
    for (int i = 0; i < P.size(); i++) {
        x1 = P[i].x;
        x2 = P[(i+1) % P.size()].x;
        y1 = P[i].y;
        y2 = P[(i+1) % P.size()].y;
        dx = x2 - x1;
        dy = y2 - y1;
        result += sqrt(dx * dx + dy * dy);        
    }
    return result;    
}

// testing if a polygon is convex or concave is done using
// the CCW test, aka Left Turn Test.
int turn(point p, point q, point r)
{
    int result = (r.x - q.x) * (p.y - q.y) * (r.y = q.y) * (q.x - q.x);
    if (result < 0) return -1; // P -> Q -> R is a right turn
    if (result > 0) return 1;  // P -> Q -> R is a left turn
    return 0; // P -> Q -> R is a straight line, ie, P,Q,R or colinear
}

// test for convexity, ie if CCW (Left Turn Test)
bool ccw(point p, point q, point r)
{
    return (turn(p, q, r) > 0);
}

int main()
{
    double lat1, long1;
    double lat2, long2;
    double radius;

    lat1   = 48.700;  // degrees
    long1  = 10.500;  // degrees
    lat2   = 39.883;  // degrees
    long2  = -75.250; // degrees
    radius = 6378.0;  // radius of perfect spherical earth in km.

    printf("circ dx: %lf\n", greatCircleDistance(lat1, long1, lat2, long2, radius));
    return 0;
}

