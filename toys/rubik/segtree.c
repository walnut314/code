#include <stdio.h>

#define TREE_SZ (100000)
int tree[2 * TREE_SZ];
int n;

void build(int arr[])
{
    for (int i = 0; i < n; i++) tree[n + i] = arr[i];
    for (int i = n-1; i > 0; --i) tree[i] = tree[i<<1] + tree[i<<1 | 1];
}

void updateTreeNode(int p, int value)
{
    tree[p+n] = value;
    p += n;
    int i = p;
    while (i > 1) {
        tree[i>>1] = tree[i] + tree[i^1];
        i >>= 1;
    }
}

int query(int l, int r)
{
    int res = 0;
    l += n; r += n;
    while (l < r) {
        if (l & 1) res += tree[l++];
        if (r & 1) res += tree[--r];
        l >>= 1; r >>= 1;
    }
    return res;
}

void show()
{
    for (int i = 0; i < 2*n; i++) {
        printf("[%02d]=> %02d\n", i, tree[i]);
    }
}

int main()
{
    int a[] = {1,2,3,4,5,6,7,8,9,10,11,12};
    n = sizeof(a)/sizeof(a[0]);
    build(a);
    show();
    printf("%d\n", query(1,3));
    updateTreeNode(2, 1);
    printf("%d\n", query(1,3));
}


