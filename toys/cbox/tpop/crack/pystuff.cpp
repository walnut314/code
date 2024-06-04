#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

// g++ -std=c++14 pystuff.cpp

using namespace std;

int findSmallest(vector<int>& arr)
{
    int smallest = arr[0];
    int smallest_index = 0;
    for (int i = 0; i < arr.size(); i++) {
        if (arr[i] < smallest) {
            smallest = arr[i];
            smallest_index = i;
        }
    }
    return smallest_index;
}

void show(const vector<int>& arr)
{
    for (const auto& v : arr)
        printf("%d ", v);
    printf("\n");
}

vector<int> selsort(vector<int>& arr)
{
    vector<int> newArr;
    int len = arr.size();
    for (int i = 0; i < len; i++) {
        int smallest = findSmallest(arr);
        printf("smallest: %d -> %d\n", smallest, arr[smallest]);
        int ele = arr[smallest];
        newArr.push_back(ele);
        arr.erase(arr.begin() + smallest);
    }
    show(arr);
    return newArr;
}

int main()
{
    vector<int> arr = {7,1,3,9,4};
    show(arr);
    vector<int> newarr = selsort(arr);
    show(newarr);
}
