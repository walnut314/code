#include <stdio.h>
#include <stdlib.h>


void show(int a[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}

void insort(int a[], int n) {
    for (int i = 1 ; i < n; i++) {
        int j = i;
        while (j > 0 && a[j] < a[j-1]) {
            int t = a[j];
            a[j] = a[j-1];
            a[j-1] = t;
            j--;
        }
    }
}

int main() {
    int a[] = {5,2,1,9,3};
    int len = sizeof(a)/sizeof(a[0]);
    show(a, len);
    insort(a, len);
    show(a, len);
}
