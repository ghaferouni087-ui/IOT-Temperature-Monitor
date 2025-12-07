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

int main() {
    float seuil_min=0;
    float seuil_max=25;
    printf("Journalisation\n");
    printf("----------------------------------------------------------------------\n");
    
    ajouter_mesure_journal(22.5);
    comparaison(22.5,seuil_min,seuil_max);
    ajouter_mesure_journal(26.8);
    comparaison(26.8,seuil_min,seuil_max);
    ajouter_mesure_journal(17.9); 
    comparaison(17.9,seuil_min,seuil_max);
    
    ecrire_journal_dans_fichier();

    liberer_journal();

    return 0;
}
