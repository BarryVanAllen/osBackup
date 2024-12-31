#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "simulation.h"
#include "affichage.h"
#include "utils.h"

#define NB_PILOTES 20

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s [all|free_practice|qualification|course]\n", argv[0]);
        return 1;
    }

    const char *session_choice = argv[1];
    int start_session = 0, end_session = 2;

    if (strcmp(session_choice, "all") == 0) {
        start_session = 0;
        end_session = 2;
    } else if (strcmp(session_choice, "free_practice") == 0) {
        start_session = 0;
        end_session = 0;
    } else if (strcmp(session_choice, "qualification") == 0) {
        start_session = 1;
        end_session = 1;
    } else if (strcmp(session_choice, "course") == 0) {
        start_session = 2;
        end_session = 2;
    } else {
        printf("Session invalide : %s. Choisissez entre all, free_practice, qualification, ou course.\n", session_choice);
        return 1;
    }

    // Crée la mémoire partagée
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

    // Initialisation des pilotes
    for (int i = 0; i < NB_PILOTES; i++) {
        snprintf(mp->pilotes[i].nom, sizeof(mp->pilotes[i].nom), "Pilote %d", i + 1);
        mp->pilotes[i].temps_meilleur_tour = 0.0;
        mp->pilotes[i].dernier_temps_tour = 0.0;
    }

    const char *sessions[] = {"Essais Libres", "Qualifications", "Course"};
    int nb_tours[] = {20, 15, 50};

    // Exécuter les sessions selon la commande choisie
    for (int s = start_session; s <= end_session; s++) {
        simulate_session(mp, sessions[s], nb_tours[s], NB_PILOTES);
    }

    // Détachement et destruction de la mémoire partagée
    if (shmdt(mp) == -1) {
        perror("Erreur de détachement de mémoire partagée");
    }
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("Erreur de destruction de mémoire partagée");
    }

    return 0;
}
