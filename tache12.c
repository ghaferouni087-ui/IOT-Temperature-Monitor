#include <stdio.h>
#include <stdlib.h>
typedef struct {
    float seuil_min;
    float seuil_max;
    int intervalle_mesure;
} Config;

int ecrire_config(Config *config) {
    FILE *f = fopen("config.txt", "w");
    if (f == NULL) return -1;
    fprintf(f, "%f\n%f\n%d\n", config->seuil_min, config->seuil_max, config->intervalle_mesure);
    fclose(f);
    return 0;
}
int lire_config(Config *config) {
    FILE *f = fopen("config.txt", "r");
    if (f == NULL) return -1;
    fscanf(f, "%f\n%f\n%d\n", &config->seuil_min, &config->seuil_max, &config->intervalle_mesure);
    fclose(f);
    return 0;
}
struct Noeud {
    float temperature;
    struct Noeud *suivant;
};

typedef struct {
    struct Noeud *tete;
    struct Noeud *queue;
} File;

File* creer_file() {
    File *f = malloc(sizeof(File));
    f->tete = NULL;
    f->queue = NULL;
    return f;
}

void enfiler(File *f, float temperature) {
    struct Noeud *n = malloc(sizeof(struct Noeud));
    n->temperature = temperature;
    n->suivant = NULL;

    if (f->queue == NULL) {
        f->tete = n;
        f->queue = n;
    } else {
        f->queue->suivant = n;
        f->queue = n;
    }
}

float defiler(File *f) {
    if (f->tete == NULL) return -1;

    struct Noeud *tmp = f->tete;
    float val = tmp->temperature;

    f->tete = f->tete->suivant;

    if (f->tete == NULL)
        f->queue = NULL;

    free(tmp);
    return val;
}

int est_vide(File *f) {
    return f->tete == NULL;
}

float capteur() {
    return (rand() % 500) / 10.0;  
}
void afficher(File *f) {
    struct Noeud *n = f->tete;
    while (n != NULL) {
        printf("%f ", n->temperature);
        n = n->suivant;
    }
}

