/*task 1-2*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#define CONFIG_FILE "config.txt"
#define NOMFICH "journal_mesures.txt"
#define MAXTIME 20
#define MAXMSG 100

typedef struct NoeudJournal {
    char timestamp[MAXTIME];
    float temperature;
    char message[MAXMSG];
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
    fprintf(f, "%lf\n%lf\n%d\n%d\n%d\n", 
            config->seuil_min, 
            config->seuil_max, 
            config->intervalle_mesure,
            config->file_size,
            config->alertes_consecutives);
    fclose(f);
    return 0;
}

int lire_config(Config *config) {
    FILE *f = fopen("config.txt", "r");
    if (f == NULL) return -1;
    fscanf(f, "%lf\n%lf\n%d\n%d\n%d\n", 
        &config->seuil_min, &config->seuil_max, 
        &config->intervalle_mesure,
        &config->file_size,
        &config->alertes_consecutives);
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
    /* Si on dépasse la taille max, enlever l'élément le plus ancien  */
    while (f->size > f->max_size) {
        Noeud *ancien = f->tete;
        if (!ancien) break;
        f->tete = ancien->suivant;
        if (f->tete == NULL) f->queue = NULL;
        free(ancien);
        f->size--;
    }
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
void copier_chaine_securisee(char *destination, const char *source, int taille_max) {
    int i = 0;
    while (source[i] != '\0' && i < taille_max - 1) {
        destination[i] = source[i];
        i++;
    }
    destination[i] = '\0';
}
void ajouter_mesure_journal(float temp,const char *msg) {
    NoeudJournal *nouveau_noeud = (NoeudJournal *)malloc(sizeof(NoeudJournal));
    obtenir_timestamp(nouveau_noeud->timestamp);
    nouveau_noeud->temperature = temp;
    copier_chaine_securisee(nouveau_noeud->message, msg, MAXMSG);
    nouveau_noeud->suivant = tete_journal;
    tete_journal = nouveau_noeud;
}


void ecrire_journal_dans_fichier() {
    FILE *fichier = fopen(NOMFICH, "a");
    NoeudJournal *courant = tete_journal;

    while (courant != NULL) {
        fprintf(fichier, "[%s] Temp: %.1fC -%s \n", 
                courant->timestamp, 
                courant->temperature,courant->message);
        courant = courant->suivant;
    }

    fclose(fichier);
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

const char* comparaison(float temp,float seuil_min,float seuil_max){
    if (temp>seuil_max){
        printf("alerte:temperature tros elevee\n");
        return ("alerte:niveau 1\n");
    }
    else if (temp<seuil_min){
        printf("alerte:temperature tros basse\n");
        return("alerte niveau 2\n");
    }
    else{
        printf("temperature normale\n");
        return ("niveau 0\n");
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
    if (!f || f->tete == NULL) return 0;
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
NODE *racine = NULL;
/*insertion de la temperature dans l arbre*/
NODE* insert(NODE* root, float temp) {
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

float findMin(NODE* root) {
    if (!root) return -1; /*arbre vide*/
    while (root->left != NULL)
        root = root->left;
    return root->temperature;
}

/*recherche la temperature maximale*/

float findMax(NODE* root) {
    if (!root) return -1; /*arbre vide*/
    while (root->right != NULL)
        root = root->right;
    return root->temperature;
}

/*parcours infixe pour produire un tableau trie*/

void inorder(NODE* root, float* arr , int* index ,int max_size) {
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
    float* arr=(float*)malloc(n * sizeof(float));
    if (arr==NULL)
    {
        printf("erreur d allocation memoire \n");
        return;
    }
    
    int index = 0;
    inorder(root,arr,&index,n);
    float sum = 0.0f;
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
        median = (arr[n/2 -1] + arr[n/2]) /2.0f;
    
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

    computeStatistics(racine);

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
    printf("rapport_journalier.txt genere");
    fclose(f);
}

/* ---------------------- Interface / Main loop ---------------------- */
void print_menu() {
    printf("\n=== Contrôleur Intelligent de Température ===\n");
    printf("1) Lancer la surveillance (mode simulation)\n");
    printf("2) Modifier la configuration (config.txt)\n");
    printf("3) Afficher la configuration\n");
    printf("4) Générer le rapport journalier\n");
    printf("5) Quitter\n");
    printf("Choix: ");
}

/* --- Monitoring loop --- */
void monitoring_loop(Config *cfg,int looplength) {
    int i= 0;
    printf("Lancement de la surveillance (Ctrl+C pour arrêter)...\n");
    File *window = creer_file();
    if (!window) {
        printf("Impossible de créer la file d'alertes.\n");
        return;
    }

    while (i<looplength) {
        char ts[64];
        obtenir_timestamp(ts);
        double temp = capteur();
        racine = insert (racine, temp);
        int niveau = determiner_niveau(temp, cfg);

        if (niveau == 0) printf("[%s] Temp = %.2f => Normal\n", ts, temp);
        else printf("[%s] Temp = %.2f => Niv%d\n", ts, temp, niveau);

        /* écrire dans journal persistant et mémoire */
        if (niveau == 0) ajouter_mesure_journal(temp,"NORMAL");
        else {
            char msg[64];
            snprintf(msg, sizeof(msg), "ALERTE_NIV_%d", niveau);
            ajouter_mesure_journal(temp, msg);
        }

        /* file d'alertes */
        Alerte a;
        a.niveau = niveau;
        a.temperature = temp;
        snprintf(a.timestamp, sizeof(a.timestamp) - 1,ts);
        enfiler(window, a);

        /* validation par nombre d'alertes consécutives */
        if (niveau > 0) {
            int consec = count_alertes_consecutive(window, niveau);
            if (consec >= cfg->alertes_consecutives) {
                printf("-> Détection confirmée: %d mesures consécutives de niveau >= %d\n", consec, niveau);
                gerer_les_alertes(niveau);
            } else {
                printf("-> Alerte détectée mais non confirmée (%d/%d)\n", consec, cfg->alertes_consecutives);
            }
        }
        ecrire_journal_dans_fichier();
        i++;
#ifdef _WIN32 
        Sleep(cfg->intervalle_mesure * 1000); 
#else 
        sleep(cfg->intervalle_mesure);
#endif 
    }

    free(window);
}



/* --- Menu principal --- */
int main() {
    Config cfg;
    config_init_default(&cfg);
    if (lire_config(&cfg) == 0) {
        printf("Config chargée depuis %s\n", CONFIG_FILE );
    } else {
        printf("Utilisation de la configuration par défaut.\n");
    }

    while (1) {
        print_menu();
        int choice = 0;
        scanf("%d", &choice);
        if (choice == 1) {
            printf("La longueur de la simulation:");
            int looplength; scanf("%d",&looplength);
            printf("Démarrage en mode simulation.\n");
            monitoring_loop(&cfg,looplength);
        } else if (choice == 2) {
            char line[256];
            printf("Modifier configuration (taper Entrée après chaque valeur)\n");
            printf("seuil_min (actuel %.2f) : ", cfg.seuil_min);
            if (scanf("%s", line) == 1) cfg.seuil_min = atof(line);
            printf("seuil_max (actuel %.2f) : ", cfg.seuil_max);
            if (scanf("%s", line) == 1) cfg.seuil_max = atof(line);
            printf("intervalle_mesure en secondes (actuel %d) : ", cfg.intervalle_mesure);
            if (scanf("%s", line) == 1) cfg.intervalle_mesure = atoi(line);
            printf("file_size (file) (actuel %d) : ", cfg.file_size);
            if (scanf("%s", line) == 1) cfg.file_size = atoi(line);
            printf("alertes_consecutives (actuel %d) : ", cfg.alertes_consecutives);
            if (scanf("%s", line) == 1) cfg.alertes_consecutives = atoi(line);

            if (cfg.seuil_min >= cfg.seuil_max) {
                printf("Attention: seuil_min >= seuil_max. Correction automatique.\n");
                cfg.seuil_max = cfg.seuil_min + 1.0;
            }
            ecrire_config(&cfg);
            printf("Configuration sauvegardée dans %s\n", CONFIG_FILE );
        } else if (choice == 3) {
            printf("Configuration actuelle:\n");
            printf("  seuil_min = %.2f\n", cfg.seuil_min);
            printf("  seuil_max = %.2f\n", cfg.seuil_max);
            printf("  intervalle_mesure = %d s\n", cfg.intervalle_mesure);
            printf("  file_size = %d\n", cfg.file_size);
            printf("  alertes_consecutives = %d\n", cfg.alertes_consecutives);
        } else if (choice == 4) {
            generer_rapport_quotidien(&cfg);
        } else if (choice == 5) {
            printf("Au revoir.\n");
            ecrire_journal_dans_fichier();
            liberer_journal();
            break;
        } else {
            printf("Choix invalide.\n");
        }
    }

    return 0;
}
