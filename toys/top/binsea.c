#include <stdio.h> 
 
// A iterative binary search function. It returns
// location of x in given array arr[l..r] if present,
// otherwise -1
int binarySearch(int arr[], int l, int r, int x, int dups)
{
    while (l <= r) {
        int m = l + (r - l) / 2;
 
        // Check if x is present at mid 
        if (arr[m] == x) {
            if (dups == 1) {
                int nx = m;
                while (arr[--nx] == x);
                nx++;
                printf("%d @ %d\n", arr[nx], nx);
                do {
                    printf(" %d,", arr[nx]);
                } while (arr[++nx] == x);
            }
            return m;
        }
        // If x greater, ignore left half
        if (arr[m] < x)
            l = m + 1;
        // If x is smaller, ignore right half
        else
            r = m - 1;
    }
 
    // if we reach here, then element was not present
    return -1;
}
  
int main(void)
{
    int arr[] = { 2, 3, 4, 4, 4, 10, 40 };
    int n = sizeof(arr) / sizeof(arr[0]);
    int x = 4;
    int find_dups = 1;
    int result = binarySearch(arr, 0, n - 1, x, find_dups);
    (result == -1) ? printf("Element %d is not present"
                            " in array\n", x)
                   : printf("Element %d is present at "
                            "index %d\n",
                            x, result);
    return 0;
}

