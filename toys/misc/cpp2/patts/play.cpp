#include <bits/stdc++.h>
using namespace std;

int main()
{
    int A[] = {5,2,8,9,4};
    int B[] = {3,2,9,5};

    unordered_set<int> setA;
    for (int i: A) {
        setA.insert(i);
    }    

    set<int> setB;
    for (int i: B) {
        auto it = setA.find(i);
        if (it != setA.end()) setB.insert(i);
    }

    for (auto it = setB.begin(); it != setB.end(); it++) {
        cout << *it << endl;
    }

}

