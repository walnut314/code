#include <stdio.h>
#include <cstdlib>

void update(int *a,int *b) {
    // Complete this function    
    //a = a + b;
    int sum = *a + *b;
    //b = |a - b|;
    int diff = abs(*a - *b);
    *a = sum;
    *b = diff;

}

int main() {
    int a, b;
    int *pa = &a, *pb = &b;
    
    scanf("%d %d", &a, &b);
    update(pa, pb);
    printf("%d\n%d", a, b);

    return 0;
}

