#include "util.h"

dpoint_s ilineLineIntersection(iline line1, iline line2)
{ 
    double a1 = line1.p2.y - line1.p1.y; 
    double b1 = line1.p1.x - line1.p2.x; 
    double c1 = a1 * (line1.p1.x) + b1 * (line1.p1.y); 
  
    double a2 = line2.p2.y - line2.p1.y; 
    double b2 = line2.p1.x - line2.p2.x; 
    double c2 = a2 * (line2.p1.x) + b2 * (line2.p1.y); 
  
    double determinant = a1 * b2 - a2 * b1; 
    if (determinant == 0) return {FLT_MAX, FLT_MAX};
    else { 
        double x = (b2 * c1 - b1 * c2) / determinant; 
        double y = (a1 * c2 - a2 * c1) / determinant; 
        return {x, y};
    } 
} 

dpoint_s dlineLineIntersection(dline line1, dline line2)
{ 
    double a1 = line1.p2.y - line1.p1.y; 
    double b1 = line1.p1.x - line1.p2.x; 
    double c1 = a1 * (line1.p1.x) + b1 * (line1.p1.y); 
  
    double a2 = line2.p2.y - line2.p1.y; 
    double b2 = line2.p1.x - line2.p2.x; 
    double c2 = a2 * (line2.p1.x) + b2 * (line2.p1.y); 
  
    double determinant = a1 * b2 - a2 * b1; 
    if (determinant == 0) return {FLT_MAX, FLT_MAX};
    else { 
        double x = (b2 * c1 - b1 * c2) / determinant; 
        double y = (a1 * c2 - a2 * c1) / determinant; 
        return {x, y};
    } 
} 

int main() 
{ 
    dline line1 = {1,1,4,4}; // x1,y1,x2,y2
    dline line2 = {1,8,2,4}; // x1,y1,x2,y2

    iline line3 = {1,1,4,4}; // x1,y1,x2,y2
    iline line4 = {1,8,2,4}; // x1,y1,x2,y2
    
    dpoint_s isect1 = dlineLineIntersection(line1, line2);
    dpoint_s isect2 = ilineLineIntersection(line3, line4);
  
    if (isect1.x == FLT_MAX && isect1.y==FLT_MAX) { 
        printf("The given lines AB and CD are parallel.\n");
    } else { 
        printf("intersection at: (%lf, %lf)\n", isect1.x, isect1.y);
    } 
    if (isect2.x == FLT_MAX && isect2.y==FLT_MAX) { 
        printf("The given lines AB and CD are parallel.\n");
    } else { 
        printf("intersection at: (%lf, %lf)\n", isect2.x, isect2.y);
    } 
    return 0; 
}
