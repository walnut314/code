#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

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
    vector<int> newArr(arr.size());
    for (int i = 0; i < arr.size(); i++) {
        int smallest = findSmallest(arr);
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
    //show(newarr);

#if 0
    int list[] = {7,1,3,9,4};
    int len = sizeof(list)/sizeof(list[0]);
    show(list, len);
    int *newarr = selsort(list, len);
    show(newarr, len);
#endif
}
