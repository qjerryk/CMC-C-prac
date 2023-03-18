#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


struct tree_node{
    int key;
    struct tree_node *left, *right;
};



void init_node(struct tree_node **node, int value){
    *node = (struct tree_node*) malloc(sizeof(struct tree_node));
    if (!(*node))
        return;
    (*node) -> key = value;
    (*node) -> left = (*node) -> right = NULL;
}

void kill_tree(struct tree_node **tree){
    if (!(*tree)){
        if ((*tree) -> right){
            kill_tree(&((*tree) -> right));
        }
        if ((*tree) -> left){
            kill_tree(&((*tree) -> left));
        }
        free(*tree);
    }
}

struct tree_node* insert_value(struct tree_node **tree, int value){
    struct tree_node *curr = *tree;
    struct tree_node *pred = NULL;
    int lr = 0;
    while (curr != NULL){
        if (curr -> key > value){
            pred = curr;
            curr = curr -> left;
            lr = -1;
        }else if (curr -> key < value){
            pred = curr;
            curr = curr -> right;
            lr = 1;
        }else{
            return *tree;
        }

    }
    init_node(&curr, value);
    if (lr == -1){
        pred -> left = curr;
    }else if(lr == 1){
        pred -> right = curr;
    }else{
        *tree = curr;
    }
    //if (lr)
      //  printf("%p %d\n", curr , curr -> key);
    return *tree;
}

int finding_mins(struct tree_node *tree, int *min, int *second_min){
    struct tree_node *pred = NULL, *curr = tree;
    *second_min = INT_MAX;
    int tmp = INT_MAX;
    //printf("curr : %d", curr -> key);
    while (curr -> left != NULL){
        pred = curr;
        curr = curr -> left;
    }
    if (pred) {
        *second_min = pred->key;
        if (curr -> right != NULL) {
            tmp = finding_mins(curr->right, min, second_min);
            *second_min = tmp;
        }
    }else{
        if (curr -> right) {
            *second_min = finding_mins(curr->right, min, second_min);
        }

    }
    *min = curr -> key;
    return curr -> key;
}

void reading(){
    int value = 0;
    int min = 0, second_min = 0;
    struct tree_node *tree;
    while (scanf("%d", &value) == 1){
        insert_value(&tree, value);
    }
	if (tree){
		finding_mins(tree, &min, &second_min);
		printf("%d %d\n", min, second_min);
	}
    kill_tree(&tree);
}

int main(){
    reading();
    return 0;
}
