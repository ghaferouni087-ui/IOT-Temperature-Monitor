#include <stdio.h>

typedef struct {
    double seuil_min;
    double seuil_max;
    int intervalle_mesure; 
    int file_size; 
    int alertes_consecutives; 
} Config;

void config_init_default(Config *c) {
    c->seuil_min = 15.0;
    c->seuil_max = 30.0;
    c->intervalle_mesure = 5;
    c->file_size = 10;
    c->alertes_consecutives = 3;
}

typedef struct {
    int niveau; 
    double temperature;
    char timestamp[64];
} Alerte;

typedef struct Noeud {
    Alerte a;
    struct Noeud *next;
} Noeud;

typedef struct {
    Noeud *tete;
    Noeud *queue;
    int size;
    int max_size; 
} File;


int count_alertes_consecutive(File *q, int niveau) {
    if (!q->queue) return 0;
    int count = 0;
    Noeud *l = q->tete;
    while (l) {
        if (l->a.niveau >= niveau) count++;
        else count=0;
        l = l->next;
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

