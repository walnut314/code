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
    int result = (r.x - q.x) * (p.y - q.y) - (r.y - q.y) * (p.x - q.x);
    if (result < 0) return -1; // P -> Q -> R is a right turn
    if (result > 0) return  1; // P -> Q -> R is a left turn
    return 0; // P -> Q -> R is a straight line, ie, P,Q,R or colinear
}

// test for convexity, ie if CCW (Left Turn Test)
bool ccw(point p, point q, point r)
{
    return (turn(p, q, r) > 0);
}

// convex hull using Graham's Scan
point pivot; // global variable
vector<point> polygon, CH; // CH == convex hull
int area2(point a, point b, point c)
{
    return (a.x * b.y - a.y * b.x +
            b.x * c.y - b.y * c.x +
            c.x * a.y - c.y * a.x);
}

// distance between 2 points
int dist2(point a, point b)
{
    int dx = a.x - b.x,
        dy = a.y - b.y;
    return (dx * dx + dy * dy);
}

// angle sorting function
bool angle_cmp(point a, point b)
{
    if (area2(pivot, a, b) == 0) // colinear
        return dist2(pivot, a) < dist2(pivot, b); // which one closer
    int d1x = a.x - pivot.x, d1y = a.y - pivot.y;
    int d2x = b.x - pivot.x, d2y = b.y - pivot.y;
    return (atan2((double)d1y, (double)d1x) -
            atan2((double)d2y, (double)d2x)) < 0;
}

vector<point> GrahamScan(vector<point> Polygon)
{
    // first, find P0 = point with lowest Y and if tie: rightmost X
    int i, P0 = 0, N = Polygon.size();
    for (i = 1; i < N; i++) {
        if (Polygon[i].y < Polygon[P0].y ||
           (Polygon[i].y == Polygon[P0].y && Polygon[i].x > Polygon[P0].x)) {
            P0 = i;
        }
    }

    point temp = Polygon[0]; // swap selected vertex with Polygon[0]
    Polygon[0] = Polygon[P0];
    Polygon[P0] = temp;

    // second, sort points by angle wrt P0, skipping Polygon[0]
    pivot = Polygon[0]; // use this global as reference
    sort(++Polygon.begin(), Polygon.end(), angle_cmp);

    // third, the ccw tests
    stack<point> S;
    point prev, now;
    S.push(Polygon[N-1]); // put two starting vertices into stack S
    S.push(Polygon[0]);

    i = 1; // and start checking the rest
    while (i < N) {
        now = S.top();
        S.pop(); prev = S.top(); S.push(now); // trick to get the 2nd itme from top  of S
        if (ccw(prev, now, Polygon[i])) { // if these 3 points make a left turn
            S.push(Polygon[i]); // accept
            i++;
        } else { // otherwise
            S.pop(); // pop this point until we have a left turn
        }
    }

    vector<point> ConvexHull;
    while (!S.empty()) {
        ConvexHull.push_back(S.top());
        S.pop();
    }
    ConvexHull.pop_back(); // the last one is a duplicate of the first one

    return ConvexHull;
}

struct line { point p1, p2; };
int intersect(line line1, line line2)
{
    return 
        ((ccw(line1.p1, line1.p2, line2.p1) * ccw(line1.p1, line1.p2, line2.p2)) <= 0)
            &&
        ((ccw(line2.p1, line2.p2, line1.p1) * ccw(line2.p1, line2.p2, line1.p2)) <= 0);
}

void test_greatCircle()
// UVa 535 Globetrotter
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
}

void test_GrahamScan()
// UVa 11626 - Convex Hull
{
    vector<point> Polygon;
    Polygon.push_back(point{1,1});
    Polygon.push_back(point{1,-1});
    Polygon.push_back(point{0,0});
    Polygon.push_back(point{-1,-1});
    Polygon.push_back(point{-1,1});

    vector<point> CH = GrahamScan(Polygon);
    for (int i = 0; i < CH.size(); i++) {
        printf("%d %d\n", CH[i].x, CH[i].y);
    }
}

void test_IntersectingLines()
{
    // 0 0 4 4 0 4 4 0
    // 5 0 7 6 1 0 2 3
    // 5 0 7 6 3 -6 4 -3
    // 2 0 2 27 1 5 18 5
    // 0 3 4 0 1 2 2 5

    line line1 = {2,0,2,27}; // x1,y1,x2,y2
    line line2 = {1,5,18,5}; // x1,y1,x2,y2

    bool inter = intersect(line1, line2);
    printf("intersect: %d\n", inter);
}

int main()
{
    //test_greatCircle();
    //test_GrahamScan();
    test_IntersectingLines();
    return 0;
}

