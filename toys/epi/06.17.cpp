#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <time.h>
//#include <algorithm>

using namespace std;

/*
 *  Compute a random permutation
 *
 *  Design an algorithm that creates uniformly random permutations of
 *  [0,1,...,n-1]. You are given a random number generator that returns
 *  integers in the set [0,1,...,n-1] with equal probability; use as
 *  few calls to it as possible.
 *
 *
 */

void show(vector<int>& A) {
    cout << "<" ;
    for (int i = 0; i < A.size(); ++i) {
    }
    cout << ">" << endl;
}

int ZeroOneRandom()
{
    double res = rand() / double(RAND_MAX);
    return (res < 0.50 ? 0 : 1); 
}

int UniformRandom(int a, int b)
{
    int t = b - a + 1, res;
    do {
        res = 0;
        for (int i = 0; (1 << i) < t; ++i) {
            res = res * 2 | ZeroOneRandom();
        }
    } while (res >= t);
    return (a + res);
}
/*
 *  Basically, for each element from left to right, you generate all the 
 *  permutations of the remaining elements. You can do this recursively, 
 *  (or iteratively if you like pain) until you get to the last element 
 *  at which point there is only one possible order.
 *
 *  So, given a list: [1,2,3,4]
 *  You just generate all permutations that start with 1, then all the 
 *  permutations that start with 2, then with 3 and 4.
 * 
 *  This effectively reduces the problem from one of finding permutations 
 *  of a list of four elements to a list of three elements. Once you continue 
 *  reducing to 2 and then 1 element, you have all of them.
 *
 */
void RandomPermutation(vector<int>& A, int k)
{
    srand((unsigned)time(NULL));
    for (int i = 0; i < k; ++i) {
        int r = UniformRandom(i, A.size()-1);
        int tmp = A[r];
        A[r] = A[i];
        A[i] = tmp;
    }
}

/* create a P vector of 0..k permutations,
 * and randomly select the element
 */
vector<vector<int>> AllPerms;

void dump_all_perms()
{
    int i, j;
    cout << "outer size: " << AllPerms.size() << endl;
    for (i = 0; i < AllPerms.size(); ++i) {
        cout << "inner size: " << AllPerms[i].size() << endl;
        //for (j = 0; j < AllPerms[i].size(); ++j) {
        //    cout << AllPerms[i][j] << ", ";
        //}
        //cout << endl;
    }
    cout << endl;
}

void register_perm(vector<int>& array)
{
    static int perm = 0;
    int i;
    for (i = 0; i < array.size(); ++i) {
        AllPerms[perm].push_back(array[i]);
    }
}

void my_swap(vector<int>& A, int i, int j)
{
    int tmp = A[i];
    A[i] = A[j];
    A[j] = tmp;
}

void permute(vector<int>& A, int k, int size)
{
    int i;
    if (k == 0) {
        show(A);
        //register_perm(array);
    } else {
        for (i = k-1; i >= 0; --i) { 
           my_swap(A, i, k-1);
           permute(A, k-1, size);
           my_swap(A, i, k-1);
       }
    }
}

int array[9] = {1,2,3,4,5,6,7,8,9};

void swap2(int x, int y){
    int tmp = array[x];
    array[x]=array[y];
    array[y]=tmp;

    return;
}
void printArray(int size){
    int i;
    for (i=0;i<size;i++)
        std::cout << array[i] << " ";
    std::cout << std::endl;
}

void permute2(int k,int size){
    int i;
    if (k==0)
        printArray(size);
    else{
        for (i=k-1;i>=0;i--){
            swap2(i,k-1);
            permute2(k-1,size);
            swap2(i,k-1);
        }
    }
}

int main()
{
    vector<int> A = {1,2,3,4,5};
    int k = 2;
    //RandomPermutation(A, k);
    //permute(A, 0, A.size());
    //dump_all_perms();
    //permute2(4, 4);
    permute(A, 4, 4);
}

