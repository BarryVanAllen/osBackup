#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>

#define NB_PILOTES 20
#define NB_TOURS_QUALIF 10
#define BASE_TEMPS 84.000 // Temps de base en secondes (1:24.000)

typedef struct {
    char nom[50];
    float temps_meilleur_tour;
    float dernier_temps_tour;
} Pilote;

typedef struct {
    Pilote pilotes[NB_PILOTES];
    int nbrLect;             // Nombre de lecteurs en cours
    sem_t mutex;             // Protection des écrivains
    sem_t mutLect;           // Protection du compteur de lecteurs
} MemoirePartagee;

// Fonction pour générer un temps de tour réaliste
float generer_temps_tour(float base_temps, int difficulte) {
    // Variation aléatoire entre -2.000 et +2.000 secondes
    float variation = ((rand() % 4000) - 2000) / 1000.0;
    return base_temps + variation;
}

// Fonction de tri des pilotes par meilleur temps
void tri_pilotes(Pilote pilotes[], int nb_pilotes) {
    for (int i = 0; i < nb_pilotes - 1; i++) {
        for (int j = 0; j < nb_pilotes - i - 1; j++) {
            if (pilotes[j].temps_meilleur_tour > pilotes[j + 1].temps_meilleur_tour) {
                Pilote temp = pilotes[j];
                pilotes[j] = pilotes[j + 1];
                pilotes[j + 1] = temp;
            }
        }
    }
}

void afficher_resultats(Pilote pilotes[], int nb_pilotes, const char *phase) {
    printf("\033[H\033[J");
    printf("--- Résultats : %s ---\n", phase);
    for (int i = 0; i < nb_pilotes; i++) {
        int minutes = (int)(pilotes[i].temps_meilleur_tour / 60);
        float seconds = pilotes[i].temps_meilleur_tour - (minutes * 60);
        printf("%d. Pilote: %s | Meilleur temps: %d:%.3f\n",
               i + 1, pilotes[i].nom, minutes, seconds);
    }
}

void executer_phase_qualification(MemoirePartagee *mp, int nb_pilotes, const char *phase) {
    for (int tour = 1; tour <= NB_TOURS_QUALIF; tour++) {
        pid_t pid;
        for (int i = 0; i < nb_pilotes; i++) {
            pid = fork();
            if (pid == 0) {
                srand(getpid() + time(NULL));
                float temps_tour = generer_temps_tour(BASE_TEMPS, 5);

                sem_wait(&mp->mutex); // Section critique pour les écrivains
                mp->pilotes[i].dernier_temps_tour = temps_tour;
                if (mp->pilotes[i].temps_meilleur_tour == 0.0 || temps_tour < mp->pilotes[i].temps_meilleur_tour) {
                    mp->pilotes[i].temps_meilleur_tour = temps_tour;
                }
                sem_post(&mp->mutex); // Fin de la section critique
                exit(0);
            }
        }

        for (int i = 0; i < nb_pilotes; i++) {
            wait(NULL);
        }

        // Section critique pour les lecteurs
        sem_wait(&mp->mutLect);
        mp->nbrLect++;
        if (mp->nbrLect == 1) {
            sem_wait(&mp->mutex); // Premier lecteur bloque les écrivains
        }
        sem_post(&mp->mutLect);

        // Lecture
        tri_pilotes(mp->pilotes, nb_pilotes);
        afficher_resultats(mp->pilotes, nb_pilotes, phase);

        // Fin de la section critique pour les lecteurs
        sem_wait(&mp->mutLect);
        mp->nbrLect--;
        if (mp->nbrLect == 0) {
            sem_post(&mp->mutex); // Dernier lecteur débloque les écrivains
        }
        sem_post(&mp->mutLect);

        usleep(1000000); // Pause de 1 seconde entre les tours
    }
}

void qualification(MemoirePartagee *mp) {
    // Q1
    executer_phase_qualification(mp, NB_PILOTES, "Q1");
    printf("Éliminés après Q1 :\n");
    for (int i = 15; i < NB_PILOTES; i++) {
        printf("%d. Pilote: %s\n", i + 1, mp->pilotes[i].nom);
    }

    // Q2
    executer_phase_qualification(mp, 15, "Q2");
    printf("Éliminés après Q2 :\n");
    for (int i = 10; i < 15; i++) {
        printf("%d. Pilote: %s\n", i + 1, mp->pilotes[i].nom);
    }

    // Q3
    executer_phase_qualification(mp, 10, "Q3");
    printf("Résultats finaux de Q3 (Top 10) :\n");
    for (int i = 0; i < 10; i++) {
        printf("%d. Pilote: %s\n", i + 1, mp->pilotes[i].nom);
    }
}

int main() {
    key_t key = ftok("f1_simulation", 65);
    int shmid = shmget(key, sizeof(MemoirePartagee), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("Erreur de création de mémoire partagée");
        exit(1);
    }

    MemoirePartagee *mp = (MemoirePartagee *)shmat(shmid, NULL, 0);
    if (mp == (void *)-1) {
        perror("Erreur de rattachement de mémoire partagée");
        exit(1);
    }

    sem_init(&mp->mutex, 1, 1);
    sem_init(&mp->mutLect, 1, 1);
    mp->nbrLect = 0;

    // Initialisation des pilotes
    for (int i = 0; i < NB_PILOTES; i++) {
        snprintf(mp->pilotes[i].nom, sizeof(mp->pilotes[i].nom), "Pilote %d", i + 1);
        mp->pilotes[i].temps_meilleur_tour = 0.0;
        mp->pilotes[i].dernier_temps_tour = 0.0;
    }

    qualification(mp);

    sem_destroy(&mp->mutex);
    sem_destroy(&mp->mutLect);

    if (shmdt(mp) == -1) {
        perror("Erreur de détachement de mémoire partagée");
    }
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("Erreur de destruction de mémoire partagée");
    }

    return 0;
}
