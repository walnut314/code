#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;


class Matrix
{
public:
    vector<vector<int>> a;
    Matrix operator+(const Matrix& mtx) {
        Matrix sum(*this);
        for (unsigned int i = 0; i < sum.a.size(); ++i) {
            for (unsigned int j = 0; j < sum.a[i].size(); ++j) {
                sum.a[i][j] += mtx.a[i][j];
            }
        }
        return sum;
    }
    void show() {
        vector< vector<int> >::iterator row;
        vector<int>::iterator col;
        for (row = a.begin(); row != a.end(); row++) {
            for (col = row->begin(); col != row->end(); col++) {
                cout << *col << endl;// do stuff ...
            }
            cout << endl;
        }
    }
};

int main () {
   int cases,k;
   cin >> cases;
   for(k=0;k<cases;k++) {
      Matrix x;
      Matrix y;
      Matrix result;
      int n,m,i,j;
      cin >> n >> m;
      for(i=0;i<n;i++) {
         vector<int> b;
         int num;
         for(j=0;j<m;j++) {
            cin >> num;
            b.push_back(num);
         }
         x.a.push_back(b);
      }
      for(i=0;i<n;i++) {
         vector<int> b;
         int num;
         for(j=0;j<m;j++) {
            cin >> num;
            b.push_back(num);
         }
         y.a.push_back(b);
      }
      result = x+y;
      for(i=0;i<n;i++) {
         for(j=0;j<m;j++) {
            cout << result.a[i][j] << " ";
         }
         cout << endl;
      }
   }  
   return 0;
}

