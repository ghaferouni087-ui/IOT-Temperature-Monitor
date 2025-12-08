/*task 1-2*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NOMFICH "journal_mesures.txt"
#define MAXTIME 20

typedef struct NoeudJournal {
    char timestamp[MAXTIME];
    float temperature;
    struct NoeudJournal *suivant;
} NoeudJournal;

typedef struct {
    double seuil_min;
    double seuil_max;
    int intervalle_mesure; 
    int file_size; 
    int alertes_consecutives; 
} Config;

typedef struct {
    int niveau; 
    double temperature;
    char timestamp[64];
} Alerte;

typedef struct Noeud {
    Alerte a; /*feha temperature,niveau,timestamp*/
    struct Noeud *suivant;
} Noeud;

typedef struct {
    Noeud *tete;
    Noeud *queue;
    int size;
    int max_size; 
} File;

typedef struct NODE {
    float temperature;
    struct NODE* left;
    struct NODE* right;
} NODE;

File* creer_file() {
    File *f = malloc(sizeof(File));
    f->tete = NULL;
    f->queue = NULL;
    return f;
}

int ecrire_config(Config *config) {
    FILE *f = fopen("config.txt", "w");
    if (f == NULL) return -1;
    fprintf(f, "%lf\n%lf\n%d\n%d\n%d\n", config->seuil_min, config->seuil_max, config->intervalle_mesure,config->file_size,config->alertes_consecutives);
    fclose(f);
    return 0;
}

int lire_config(Config *config) {
    FILE *f = fopen("config.txt", "r");
    if (f == NULL) return -1;
    fscanf(f, "%lf\n%lf\n%d\n%d\n%d\n", &config->seuil_min, &config->seuil_max, &config->intervalle_mesure,&config->file_size,&config->alertes_consecutives);
    fclose(f);
    return 0;
}

void enfiler(File *f, Alerte a) {
    Noeud *n = malloc(sizeof(Noeud));
    n->a = a;
    n->suivant = NULL;

    if (f->queue == NULL) {
        f->tete = n;
        f->queue = n;
    } else {
        f->queue->suivant = n;
        f->queue = n;
    }
    f->size++;
}

Alerte defiler(File *f) {
    Alerte a={0}; /*default*/
    if (f->tete == NULL) return a;

    Noeud *tmp = f->tete;
    a = tmp->a;
    f->tete = f->tete->suivant;
    if (f->tete == NULL)
        f->queue = NULL;

    free(tmp);
    f->size--;
    return a;
}

int est_vide(File *f) {
    return f->tete == NULL;
}

float capteur() {
    return (rand() % 500) / 10.0;  
}
void afficher(File *f) {
    Noeud *n = f->tete;
    while (n != NULL) {
        printf("temperature: %f,niveau: %d,timestamp: %s\n ", n->a.temperature,n->a.niveau,n->a.timestamp);
        n = n->suivant;
    }
}

/*task 3-4*/
NoeudJournal *tete_journal = NULL;

void obtenir_timestamp(char *ch) {
    time_t timer;
    struct tm *tm_info;

    time(&timer);
    tm_info = localtime(&timer);
    strftime(ch, MAXTIME, "%Y-%m-%d %H:%M:%S", tm_info);
}

void ajouter_mesure_journal(float temp) {
    NoeudJournal *nouveau_noeud = (NoeudJournal *)malloc(sizeof(NoeudJournal));
    obtenir_timestamp(nouveau_noeud->timestamp);
    nouveau_noeud->temperature = temp;
    nouveau_noeud->suivant = tete_journal;
    tete_journal = nouveau_noeud;
    printf("-> Mesure (%.1fC) ajoutee a la liste chainee a %s.\n", temp, nouveau_noeud->timestamp);
}

void ecrire_journal_dans_fichier() {
    FILE *fichier = fopen(NOMFICH, "a");
    NoeudJournal *courant = tete_journal;
    printf("\n--- Ecriture du journal en memoire dans %s ---\n", NOMFICH);

    while (courant != NULL) {
        fprintf(fichier, "[%s] Temp: %.1fC \n", 
                courant->timestamp, 
                courant->temperature);
        courant = courant->suivant;
    }

    fclose(fichier);
    printf("--- Ecriture terminee ---\n");
}

void liberer_journal() {
    NoeudJournal *courant = tete_journal;
    NoeudJournal *suivant;

    
    while (courant != NULL) {
        suivant = courant->suivant;
        free(courant);
        courant = suivant;
    }
    tete_journal = NULL;
    
}

void comparaison(float temp,float seuil_min,float seuil_max){
    if (temp>seuil_max){
        printf("alerte:temperature tros elevee\n");
    }
    else if (temp<seuil_min){
        printf("alerte:temperature tros basse\n");
    }
    else{
        printf("temperature normale\n");
    }
}

/*task 5*/
void config_init_default(Config *c) {
    c->seuil_min = 15.0;
    c->seuil_max = 30.0;
    c->intervalle_mesure = 5;
    c->file_size = 10;
    c->alertes_consecutives = 3;
}

int count_alertes_consecutive(File *f, int niveau) {
    if (!f->queue) return 0;
    int count = 0;
    Noeud *l = f->tete;
    while (l) {
        if (l->a.niveau >= niveau) count++;
        else count=0;
        l = l->suivant;
    }
    return count;
}

int determiner_niveau(double temp, Config *cfg) {
    if (temp < cfg->seuil_min) {
        double diff = cfg->seuil_min - temp;
        if (diff <= 2.0) return 1;
        else if (diff <= 5.0) return 2;
        else return 3;
    } 
    else if (temp > cfg->seuil_max) {
        double diff = temp - cfg->seuil_max;
        if (diff <= 2.0) return 1;
        else if (diff <= 5.0) return 2;
        else return 3;
    }
    return 0; 
}

void gerer_les_alertes(int niveau) {
    switch (niveau) {
        case 1:
            printf("ACTION: Niveau 1 -> Avertissement affiché.\n");
            break;
        case 2:
            printf("ACTION: Niveau 2 -> Activer ventilation / système modéré.\n");
            break;
        case 3:
            printf("ACTION: Niveau 3 -> URGENCE ! Activer refroidissement + alarme.\n");
            break;
        default:
            break;
    }
}

/*task 6*/
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

void compter_alertes_par_type(int *n1,int *n2,int *n3, Config *cfg) {
    *n1 = *n2 = *n3 =0;

    NoeudJournal *courant = tete_journal;
    while (courant != NULL) {
        int niveau = determiner_niveau(courant->temperature, cfg);
        if (niveau == 1 ) (*n1)++;
        else if (niveau == 2 ) (*n2)++;
        else if (niveau == 3 ) (*n3)++;
        courant = courant->suivant;
    }
}

int calculer_duree_alerte_totale(Config *cfg) {
    int duree = 0;
    int consecutif = 0;

    NoeudJournal * courant = tete_journal;
    while (courant != NULL)
    {
        int niveau = determiner_niveau(courant->temperature,cfg);
        if (niveau > 0) {
            consecutif++;
            if (consecutif >= cfg->alertes_consecutives)
                duree += cfg->intervalle_mesure;
        }
        else {
            consecutif = 0; /*reset*/
        }
        courant = courant->suivant;
    }
    return duree;
}

void generer_rapport_quotidien(Config *cfg) {
    if (tete_journal == NULL)
    {
        printf("aucune donnee disponible");
        return;
    }
    FILE *f= fopen("rapport_journalier.txt","w");
    if (!f)
    {
        printf("erreur : impossibe de creer le fchier \n");
        return;
    }

    float min = 9999;
    float max = -9999;
    float somme = 0;
    int count = 0;

    NoeudJournal *courant = tete_journal;
    while (courant != NULL)
    {
        float t = courant->temperature;
        if(t<min) min=t;
        if(t>max) max=t;

        somme += t;
        count++;
        courant = courant->suivant;
    }

    float moyenne = somme/count;


    int n1,n2,n3;
    compter_alertes_par_type(&n1,&n2,&n3,cfg);
    int duree_alerte = calculer_duree_alerte_totale(cfg);

/*rapport*/

    fprintf(f,"STATISTIQUES TEMPERTAURE\n");
    fprintf(f,"nombre de mesure : %d\n",count);
    fprintf(f,"minimum : %f \n",min);
    fprintf(f,"maximum : %f \n",max);
    fprintf(f,"moyenne : %f \n",moyenne);

    fprintf(f,"alertes detectees\n");
    fprintf(f,"niveau 1 : %d\n", n1);
    fprintf(f,"niveau 2 : %d\n", n2);
    fprintf(f,"niveau 3 : %d\n", n3);
    fprintf(f,"total alertes : %d\n", n1+n2+n3);

    fprintf(f, "duree totale en etat d alerte\n");
    fprintf(f, "%d minutes \n", duree_alerte);

    char ts[64];
    obtenir_timestamp(ts);
    fprintf(f,"rapport généré le : %s",ts);

    fclose(f);
}