#include <stdio.h>
#include <vector>

// g++ -std=c++11 grok.cpp 

using namespace std;

int binsea(int a[], int len, int item) {
    int lo=0, mid, hi=len-1, guess;
    while (lo <= hi) {
        mid = (lo+hi)/2;
        guess = a[mid];
        if (guess == item) return mid;
        else if (guess < item) {
            lo = mid+1;
        }
        else if (guess > item) {
            hi = mid-1;
        }
    }
    return -1;
}

int findSmallest(vector<int> arr) {
    auto smallest = arr[0];
    int smallest_index = 0;
    for (int i = 0; i < arr.size(); i++) {
        if (arr[i] < smallest) {
            smallest = arr[i];
            smallest_index = i;
        }
    }
    return smallest_index;
}

vector<int> selsort(vector<int> arr) {
    vector<int> newArr;
    vector<int> copiedArr = arr;
    for (auto i : copiedArr) {
        auto smallest = findSmallest(copiedArr);
        newArr.push_back(copiedArr[smallest]);
        copiedArr.erase(copiedArr.begin() + smallest);
    }
    return newArr;
}

void vecshow(vector<int> a) {
    for (auto i : a) {
        printf("%d ", i);
    }
    printf("\n");
}

int main()
{
    int a[] = {3,5,7,9,10};
    vector<int> b = {7,5,8,10,6};
    int len = sizeof(a)/sizeof(a[0]);
    int item = 33;
    printf("binsea(%d) index = %d\n", item, binsea(a, len, item));
    vecshow(b);
    auto c = selsort(b);
    vecshow(c);
}


