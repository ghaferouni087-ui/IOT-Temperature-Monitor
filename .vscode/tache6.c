#include <stdio.h>
#include <stdlib.h>

/*definition du structure*/
typedef struct NODE {
    int temperature;
    struct NODE* left;
    struct NODE* right;
} NODE;

/*insertion de la temperature dans l arbre*/
NODE* insert(NODE* root, int temp) {
    if (root == NULL) {
        NODE* newNODE = (NODE*)malloc(sizeof(NODE));
        newNODE->temperature = temp;
        newNODE->left = newNODE->right = NULL;
        return newNODE;
    }

    if (temp< root->temperature)
        root->left = insert(root->left,temp);
    else if (temp> root->temperature)
        root->right = insert(root->right, temp);

    return root;
}

/*recherche la temperature minimal*/

int findMin(NODE* root) {
    if (!root) return -1; /*arbre vide*/
    while (root->left != NULL)
        root = root->left;
    return root->temperature;
}

/*recherche la temperature maximal*/

int findMax(NODE* root) {
    if (!root) return -1; /*arbre vide*/
    while (root->right != NULL)
        root = root->right;
    return root->temperature;
}

/*parcours infixe pour produire un tableau trie*/

void inorder(NODE* root, int* arr , int* index ,int max_size) {
    if (root == NULL || *index >= max_size) return;

    inorder(root->left,arr,index,max_size);
    if (*index < max_size)
    {
        arr[(*index)++] = root->temperature;
    }
    inorder(root->right,arr,index,max_size);
}

/* compter le nombre de noeuds*/
int countNodes(NODE* root) {
    if (root == NULL) return 0;
    return 1 + countNodes (root->left) + countNodes(root->right) ;
}

/*calcul du moyen et mediane*/
void computeStatistics(NODE* root) {
    if (root == NULL)
    {
        printf("aucune donnée disponible \n");
        return;
    }
    int n = countNodes(root);
    int* arr=(int*)malloc(n * sizeof(int));
    if (arr==NULL)
    {
        printf("erreur d allocation memoire \n");
        return;
    }
    
    int index = 0;
    inorder(root,arr,&index,n);
    int sum = 0;
    for (int i = 0; i < n; i++)
        sum += arr[i];
    float average = (float)sum / n;
    float median;
    if (n == 1) {
        median = arr[0];
    }
    else if (n%2 == 1)
        median = arr[n/2];
    else
        median = (arr[n/2 -1] + arr[n/2]) /2.0;
    
    printf("Min=%d\n", arr[0]);
    printf("Max=%d\n", arr[n-1]);
    printf("moyen=%f\n", average);
    printf("Mediane=%f\n", median);
    free(arr);
}

/*liberer l arbre*/
void freeTree(NODE* root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

int main() {
    NODE* root = NULL;
    root = insert(root,25);
    root = insert(root,18);
    root = insert(root,30);
    root = insert(root,20);

    computeStatistics(root);
    
    freeTree(root);
    return 0;
}