#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <utility>
#include <set>
#include <map>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cfloat>

using namespace std;

const double PI = 3.141592;

#define REP(i, a, b) \
    for (int i = int(a); i <= int(b); i++)

#define TRvii(c, it) \
    for (vii::iterator it = (c).begin(); it != (c).end(); ++it)

#define INF         (99999)
#define MAX_NUM     (256)
#define DFS_WHITE   (-1)
#define DFS_BLACK   (1) // these are numbered as numComponents
#define DFS_GRAY    (2)
#define DFS_BLUE    (3)
#define DFS_RED     (4)
#define DFS_GREEN   (5)
#define DFS_YELLOW  (6)

typedef pair<int, int> ii;
typedef vector<vector<int>> vvi;
typedef vector<vector<bool>> vvb;
typedef vector<ii> vii;

// geometry
struct ipoint_s { int x, y; };
struct dpoint_s { double x, y; };
typedef pair<int, int> ipoint;
typedef pair<double, double> dpoint;
struct iline { ipoint_s p1, p2; };
struct dline { dpoint_s p1, p2; };

// Graphs
int E, V;
vector<vii> AdjList; // array V of pairs u and wt
vvi AdjMatrix; // matrix VxV
vvb d;
priority_queue<pair<int, ii>> EdgeList;

typedef enum {
    DIRECTED,
    UNDIRECTED
} graph_type;

graph_type gr_type;

void show_matrix();
void show_dx();

void init_graph(int v, int e) {
    AdjList.clear();
    EdgeList = priority_queue<pair<int, ii>>(); // reset
    AdjList.resize(MAX_NUM);
    AdjMatrix.resize(MAX_NUM);
    d.resize(MAX_NUM);
    REP (i, 0, MAX_NUM-1) {
        AdjMatrix[i].resize(MAX_NUM);
        d[i].resize(MAX_NUM);
        REP (j, 0, MAX_NUM-1) {
            AdjMatrix[i][j] = INF;
            d[i][j] = false;
        }
    }
}

void show_matrix()
{
    REP (i, 0, V-1) {
        REP (j, 0, V-1) {
            if (AdjMatrix[i][j] == INF) 
                printf("INF ");
            else
                printf("%03d ", AdjMatrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void show_dx()
{
    REP (i, 0, V-1) {
        REP (j, 0, V-1) {
            printf("%d ", (int) d[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void read_graph(graph_type type) { // Boiler plate graph scanner
    int wt, u, v;
    gr_type = type;
    scanf("%d %d", &V, &E);
    init_graph(V, E);
    for (int i = 0; i < E; i++) {
        scanf("%d %d %d", &u, &v, &wt);
        AdjList[u].push_back(make_pair(v, wt));
        //printf("%d %d = %d\n", u, v, wt);
        AdjMatrix[u][v] = wt;
        d[u][v] = true;
        EdgeList.push(make_pair(-wt, make_pair(u, v)));// negate wts for min heap
        if (gr_type == UNDIRECTED) {
            //printf("add undirected: %d, %d = %d\n", u, v, wt);
            AdjList[v].push_back(make_pair(u, wt));
            AdjMatrix[v][u] = wt;
            d[v][u] = true;
        }
    }
}

// Sets
vector<int> pset(1000);
void initSet(int _size) {pset.resize(_size); REP(i, 0, _size-1) pset[i] = i;}
int findSet(int i) {return (pset[i] == i) ? i : (pset[i] = findSet(pset[i]));}
void unionSet(int i, int j) {pset[findSet(i)] = findSet(j);}
bool isSameSet(int i, int j) {return findSet(i) == findSet(j);}

