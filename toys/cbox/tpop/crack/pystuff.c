#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void swap(int* xp, int* yp) 
{ 
    int temp = *xp; 
    *xp = *yp; 
    *yp = temp; 
} 

void selsort(int *arr, int len)
{
    for (int i = 0; i < len-1; i++) {
        int min_idx = i;
        for (int j = i+1; j < len; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        swap(&arr[min_idx], &arr[i]);
    }
}

void show(int *arr, int len)
{
    for (int i = 0; i < len; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main()
{
    int arr[] = {7,1,3,9,4};
    int len = sizeof(arr)/sizeof(arr[0]);
    show(arr, len);
    selsort(arr, len);
    show(arr, len);
}
