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
#include "affichage.h"
#include "file_manager.h"
#include "types.h"

#define NB_TOURS_ESSAIS 20
#define NB_TOURS_QUALIF 15
#define NB_TOURS_COURSE 50
#define BASE_TEMPS 70.0
#define VARIATION_MIN -3000 // Minimum time variation in milliseconds
#define VARIATION_MAX 3000  // Maximum time variation in milliseconds
#define BASE_TEMPS_SEC 84.000

float generer_temps_secteur() {
    return BASE_TEMPS_SEC + ((rand() % (VARIATION_MAX - VARIATION_MIN + 1)) + VARIATION_MIN) / 1000.0;
}

void generer_temps_pilote(Pilote *pilote) {
    pilote->secteur_1 = generer_temps_secteur();
    pilote->secteur_2 = generer_temps_secteur();
    pilote->secteur_3 = generer_temps_secteur();
    pilote->dernier_temps_tour = pilote->secteur_1 + pilote->secteur_2 + pilote->secteur_3;
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

// Executer phase de qualification
void executer_phase_qualification(MemoirePartagee *mp, int nb_pilotes, const char *phase) {
    for (int tour = 1; tour <= NB_TOURS_QUALIF; tour++) {
        pid_t pid;
        for (int i = 0; i < nb_pilotes; i++) {
            pid = fork();
                if (pid == 0) {
                    srand(getpid() + time(NULL));

                        sem_wait(&mp->mutex); // Section critique pour les écrivains
                        generer_temps_pilote(&mp->pilotes[i]);
    
                    // Mise à jour du meilleur temps de tour
                    if (mp->pilotes[i].temps_meilleur_tour == 0.0 || mp->pilotes[i].dernier_temps_tour < mp->pilotes[i].temps_meilleur_tour) {
                    mp->pilotes[i].temps_meilleur_tour = mp->pilotes[i].dernier_temps_tour;
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
// Qualification
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

// Write session results to a CSV file
void ecrire_resultats_csv(const char *filename, Pilote pilotes[], int nb_pilotes, const char *session) {
    char **data = malloc(nb_pilotes * sizeof(char *));
    for (int i = 0; i < nb_pilotes; i++) {
        char formatted_time[50];
        format_temps(pilotes[i].temps_meilleur_tour, formatted_time);
    
    }
    write_to_csv(filename, data, nb_pilotes, 0);
    for (int i = 0; i < nb_pilotes; i++) {
        free((void *)data[i]);
    }
    free(data);
}

float generer_temps_course() {
    int random_variation = (rand() % (VARIATION_MAX - VARIATION_MIN + 1)) + VARIATION_MIN;
    return BASE_TEMPS_SEC + (random_variation / 1000.0);  // Temps de base + variation aléatoire
}

// Fonction pour exécuter la phase de course
void executer_phase_course(MemoirePartagee *mp, int nb_pilotes) {
    for (int tour = 1; tour <= NB_TOURS_COURSE; tour++) {
        pid_t pid;
        for (int i = 0; i < nb_pilotes; i++) {
            pid = fork();
            if (pid == 0) {
                srand(getpid() + time(NULL));
                float temps_tour = generer_temps_course();
                
                sem_wait(&mp->mutex); // Section critique pour les écrivains
                mp->pilotes[i].dernier_temps_tour = temps_tour;
                // Mettez à jour les temps de chaque pilote pour simuler la course
                mp->pilotes[i].temps_course_total += temps_tour;
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

        // Lecture des résultats après chaque tour de la course
        tri_pilotes(mp->pilotes, nb_pilotes);
        afficher_resultats(mp->pilotes, nb_pilotes, "Course");

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

// Simulation de la course après les qualifications
void course(MemoirePartagee *mp) {
    // Phase de course : tous les pilotes participent
    printf("Début de la phase de course :\n");
    executer_phase_course(mp, NB_PILOTES);
    
    // Affichage des résultats finaux de la course
    printf("Résultats finaux de la course :\n");
    for (int i = 0; i < NB_PILOTES; i++) {
        printf("%d. Pilote: %s - Temps total de la course: %.3f secondes\n", 
            i + 1, mp->pilotes[i].nom, mp->pilotes[i].temps_course_total);
    }
}

// Modifications dans le main
int main() {
    // Création de la mémoire partagée et des sémaphores
    key_t key = ftok("f1_simulation", 65);
    int shmid = shmget(key, sizeof(MemoirePartagee), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("Erreur de création de mémoire partagée");
        exit(1);
    }

    // Attachement de la mémoire partagée
    MemoirePartagee *mp = (MemoirePartagee *)shmat(shmid, NULL, 0);
    if (mp == (void *)-1) {
        perror("Erreur de rattachement de mémoire partagée");
        exit(1);
    }

    sem_init(&mp->mutex, 1, 1);   // Initialisation du mutex pour protéger les écrivains
    sem_init(&mp->mutLect, 1, 1);  // Initialisation de la protection des lecteurs
    mp->nbrLect = 0;               // Aucun lecteur actif au départ

    // Initialisation des pilotes
    for (int i = 0; i < NB_PILOTES; i++) {
        snprintf(mp->pilotes[i].nom, sizeof(mp->pilotes[i].nom), "Pilote %d", i + 1);
        mp->pilotes[i].temps_meilleur_tour = 0.0;
        mp->pilotes[i].dernier_temps_tour = 0.0;
        mp->pilotes[i].temps_course_total = 0.0;  // Initialisation du temps de course
    }

    // Exécution des qualifications
    qualification(mp);

    // Exécution de la course
    course(mp);

    // Nettoyage : Détachement de la mémoire partagée et destruction des sémaphores
    shmdt(mp);
    shmctl(shmid, IPC_RMID, NULL);  // Suppression de la mémoire partagée
    
    sem_destroy(&mp->mutex);
    sem_destroy(&mp->mutLect);

    return 0;
}


