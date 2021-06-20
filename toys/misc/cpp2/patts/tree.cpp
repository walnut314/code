#include <bits/stdc++.h>
using namespace std;

typedef struct _tnode {
    int key;
    struct _tnode *left;
    struct _tnode *right;
} tnode;

#define MAX_DEPTH 10
// print 2D tree in reverse inorder traversal 
void print2D(tnode *root, int space) 
{ 
    if (root == NULL) return; 
    space += MAX_DEPTH; // increase dx btwn levels
    print2D(root->right, space); // right first
    printf("\n"); // print curr node after spaces
    for (int i = MAX_DEPTH; i < space; i++) printf(" ");
    printf("%d\n", root->key);
    print2D(root->left, space); // last is left
} 

tnode* create_tnode(int key)
{
    tnode *node = (tnode *) malloc(sizeof(tnode));
    node->key = key;
    node->left = node->right = NULL;
    return node;
}

tnode* insert(tnode *root, int key)
{
    if (!root)                root        = create_tnode(key);
    else if (key < root->key) root->left  = insert(root->left, key);
    else if (key > root->key) root->right = insert(root->right, key);
    else                      printf("dup %d\n", key); 
    return root;
}

void traverse(tnode *root)
{
    if (root == NULL) return;
    traverse(root->left);
    printf("%d\n", root->key);
    traverse(root->right);
}
  
int main()
{
    tnode *root = NULL; //create_tnode(3);
    for (int i = 1; i < 4; i++) {
        root = insert(root, 10*i);
    }
    root = insert(root, 4);
    root = insert(root, 1);
    root = insert(root, 2);
    root = insert(root, 3);
    //print2D(root, 0); 
    traverse(root);
}
