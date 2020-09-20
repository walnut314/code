#include "util.h"

// uses double points

// Function used to display X and Y coordinates 
// of a point 
void displayPoint(dpoint_s P) 
{ 
    printf("(%lf, %lf)\n", P.x, P.y);
} 
  
dpoint_s lineLineIntersection(dpoint_s A, dpoint_s B, dpoint_s C, dpoint_s D) 
{ 
    // Line AB represented as a1x + b1y = c1 
    double a1 = B.y - A.y; 
    double b1 = A.x - B.x; 
    double c1 = a1 * (A.x) + b1 * (A.y); 
  
    // Line CD represented as a2x + b2y = c2 
    double a2 = D.y - C.y; 
    double b2 = C.x - D.x; 
    double c2 = a2 * (C.x) + b2 * (C.y); 
  
    double determinant = a1 * b2 - a2 * b1; 
  
    if (determinant == 0) 
    { 
        // The lines are parallel. This is simplified 
        // by returning a pair of FLT_MAX 
        return {FLT_MAX, FLT_MAX};
    } 
    else
    { 
        double x = (b2 * c1 - b1 * c2) / determinant; 
        double y = (a1 * c2 - a2 * c1) / determinant; 
        return {x, y};
    } 
} 
  
// Driver code 
int main() 
{ 
    dpoint_s A {1, 1};
    dpoint_s B {4, 4};
    dpoint_s C {1, 8};
    dpoint_s D {2, 4};
  
    dpoint_s intersection = lineLineIntersection(A, B, C, D); 
  
    if (intersection.x == FLT_MAX && 
        intersection.y==FLT_MAX) 
    { 
        cout << "The given lines AB and CD are parallel.\n"; 
    } 
  
    else
    { 
        // NOTE: Further check can be applied in case 
        // of line segments. Here, we have considered AB 
        // and CD as lines 
        cout << "The intersection of the given lines AB "
                "and CD is: "; 
        displayPoint(intersection); 
    } 
  
    return 0; 
}
