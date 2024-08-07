#include "util.h"

// applications:
//     connected components undirected graph
//     flood fill - labeling connected components
//     property check
//     articulation points and bridges
//     strongly connected components in directed graph - Tarjan
//     topological sort on DAG

int dfs_num[MAX_NUM];
int dfs_parent[MAX_NUM];
int dfs_low[MAX_NUM];

void graphCheck(int u)
{
    dfs_num[u] = DFS_GRAY;
    TRvii (AdjList[u], v) {
        if (dfs_num[v->first] == DFS_WHITE) { // GRAY to WHITE
            printf(" Tree Edge (%d, %d)\n", u, v->first);
            dfs_parent[v->first] = u; // parent of these children is me
            graphCheck(v->first);
        } 
        else if (dfs_num[v->first] == DFS_GRAY) { // GRAY to GRAY
            if (v->first == dfs_parent[u])
                printf(" Bidirectional (%d, %d) - (%d, %d)\n", u, v->first, v->first, u);
            else
                printf(" Back Edge (%d, %d) (Cycle)\n", u, v->first);
        }
        else if (dfs_num[v->first] == DFS_BLACK) // GRAY to BLACK
            printf(" Forward/Cross Edge (%d, %d) (Cycle)\n", u, v->first);
    }
    dfs_num[u] = DFS_BLACK;
}

void dfs(int u)
{
    dfs_num[u] = DFS_BLACK;
    printf(" %d", u);
    TRvii (AdjList[u], v) // traversing vector of ii
        if (dfs_num[v->first] == DFS_WHITE) // avoid cycle
            dfs(v->first);
}

// find connected components in undirected graph
void find_connected()
{
    int numComponent = 0;
    REP (i, 0, V-1)
        if (dfs_num[i] == DFS_WHITE) {
            printf("Edge %d Component %d, visit:", i, ++numComponent);
            graphCheck(i);
            //dfs(i);
            printf("\n");
        }
}

void floodfill(int u, int color)
{
    dfs_num[u] = color;
    TRvii (AdjList[u], v) // traversing vector of ii
        if (dfs_num[v->first] == DFS_WHITE) // avoid cycle
            floodfill(v->first, color);
}

void flood_fill_util()
{
    int numComponent = 0;
    REP (i, 0, V-1)
        if (dfs_num[i] == DFS_WHITE) {
            printf("calling edge: %d\n", i);
            floodfill(i, ++numComponent);
        }
    REP (i, 0, V-1)
        printf("Vertex %d has color %d\n", i, dfs_num[i]);
}

int dfsNumberCounter = 0;
int dfsRoot = 0;
int rootChildren = 0;
bool articulation_vertex[MAX_NUM];
void articulationPointAndBridge(int u)
{
    dfs_low[u] = dfs_num[u] = dfsNumberCounter++; // dfs_low[u] <= dfs_num[u]
    TRvii (AdjList[u], v)
        if (dfs_num[v->first] == DFS_WHITE) { // tree edge
            dfs_parent[v->first] = u;
            if (u == dfsRoot)
                rootChildren++;
            articulationPointAndBridge(v->first);
            if (dfs_low[v->first] >= dfs_num[u]) // for articulation point
                articulation_vertex[u] = true;
            if (dfs_low[v->first] > dfs_num[u]) // for bridge
                printf(" Edge (%d, %d) is a bridge\n", u, v->first);
            dfs_low[u] = min(dfs_low[u], dfs_low[v->first]); // update dfs_low[u]
        }
        else if (v->first != dfs_parent[u]) // back edge and not direct cycle
            dfs_low[u] = min(dfs_low[u], dfs_num[v->first]); // update dfs_low[u]
}

void articulationPointAndBridge_util()
{
    dfsNumberCounter = 0;
    printf("Bridges:\n");
    REP (i, 0, V-1)
        if (dfs_num[i] == DFS_WHITE) {
            dfsRoot = i; rootChildren = 0;
            articulationPointAndBridge(i);
            articulation_vertex[dfsRoot] = (rootChildren > 1); // special case
        }
    printf("Articulation Points:\n");
    REP (i, 0, V-1)
        if (articulation_vertex[i])
            printf(" Vertex %d\n", i);
}

// tarjan
stack<int> dfs_scc;
set<int> in_stack;
void strong_connected(int u) // tarjan
{
    dfs_low[u] = dfs_num[u] = dfsNumberCounter++; // dfs_low[u] <= dfs_num[u]
    dfs_scc.push(u); in_stack.insert(u); // stores u based on order of visitation
    TRvii (AdjList[u], v) {
        if (dfs_num[v->first] == DFS_WHITE)
            strong_connected(v->first);
        if (in_stack.find(v->first) != in_stack.end()) // NOT IN - condition for update
            dfs_low[u] = min(dfs_low[u], dfs_low[v->first]); // update dfs_low[u]
    }
    if (dfs_low[u] == dfs_num[u]) {
        printf("SCC: ");
        while (!dfs_scc.empty() && dfs_scc.top() != u) {
            printf("%d ", dfs_scc.top());
            in_stack.erase(dfs_scc.top());
            dfs_scc.pop();
        }
        printf("%d\n", dfs_scc.top());
        in_stack.erase(dfs_scc.top());
        dfs_scc.pop();        
    }
}

vector<int> topologicalSort;
void topVisit(int u)
{
    dfs_num[u] = DFS_BLACK;
    TRvii (AdjList[u], v)
        if (dfs_num[v->first] == DFS_WHITE)
            topVisit(v->first);
    topologicalSort.push_back(u);
}

void topSort_util()
{
    topologicalSort.clear();
    REP (i, 0, V-1)
        if (dfs_num[i] == DFS_WHITE) {
            printf("topsort: %d\n", i);
            topVisit(i);
        }
    reverse(topologicalSort.begin(), topologicalSort.end());
    REP (i, 0, topologicalSort.size()-1)
        printf("%d\n", topologicalSort[i]);
}

int main()
{
    read_graph(UNDIRECTED);

    memset(dfs_num, DFS_WHITE, sizeof(dfs_num));
    memset(dfs_parent, DFS_WHITE, sizeof(dfs_parent));
    memset(dfs_low, DFS_WHITE, sizeof(dfs_low));
    memset(articulation_vertex, false, sizeof(articulation_vertex));

    //dfs(0);
    //find_connected();
    //flood_fill_util();
    //articulationPointAndBridge_util();

    // tarjan
    //dfsNumberCounter = 0;
    //strong_connected(0);

    topSort_util();

    printf("\n");
}

