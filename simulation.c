
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "simulation.h"
#include "affichage.h"
#include "utils.h"

void simulate_session(MemoirePartagee *mp, const char *session, int nb_tours, int nb_pilotes) {
    for (int tour = 1; tour <= nb_tours; tour++) {
        pid_t pid;
        for (int i = 0; i < nb_pilotes; i++) {
            pid = fork();
            if (pid == 0) {
                srand(getpid() + time(NULL)); // Graine unique
                float temps_tour = generer_temps_tour(70.0, 5);
                mp->pilotes[i].dernier_temps_tour = temps_tour;

                // Met à jour le meilleur temps si nécessaire
                if (mp->pilotes[i].temps_meilleur_tour == 0.0 || temps_tour < mp->pilotes[i].temps_meilleur_tour) {
                    mp->pilotes[i].temps_meilleur_tour = temps_tour;
                }
                exit(0);
            }
        }

        for (int i = 0; i < nb_pilotes; i++) {
            wait(NULL);
        }

        // Tri et affichage
        tri_pilotes(mp->pilotes, nb_pilotes);
        afficher_resultats_en_temps_reel(mp->pilotes, tour, session);
        usleep(500000);
    }
}
