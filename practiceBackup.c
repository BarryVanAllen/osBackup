"faut juste faire le lien avec les fork et l'affichage"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NB_PILOTES 10   // Nombre de pilotes
#define NB_TOURS 20     // Nombre de tours

// Définition de la structure Pilote
typedef struct {
    char nom[50];          // Nom du pilote
    float temps_meilleur_tour;  // Meilleur temps du tour
} Pilote;

// Prototypes des fonctions
float generer_temps_tour(float base_temps, int difficulte);
void simulations_essais_libres(Pilote pilotes[], char* type_session);
void tri_pilotes(Pilote pilotes[], char* type_session);
void afficher_resultats(Pilote pilotes[], char* type_session);

// Fonction pour générer un temps de tour aléatoire avec un coefficient de difficulté
float generer_temps_tour(float base_temps, int difficulte) {
    float variation = (rand() % 500) / 1000.0;  // Variation jusqu'à 0.5 secondes
    float coefficient_difficulte = 1.0 + (difficulte * 0.01);  // Ajustement en fonction de la difficulté
    return base_temps * coefficient_difficulte + variation;
}

// Fonction principale de simulation des essais libres
void simulations_essais_libres(Pilote pilotes[], char* type_session) {
    srand(time(NULL));  // Initialisation du générateur de nombres aléatoires

    // Boucle sur chaque tour
    for (int j = 0; j < NB_TOURS; j++) {
        printf("\n--- Tour %d ---\n", j + 1);

        // Boucle sur chaque pilote pour générer leur temps de tour
        for (int i = 0; i < NB_PILOTES; i++) {
            // Générer un temps de tour pour chaque pilote
            float temps_tour = generer_temps_tour(70.0, 5);  // Exemple : base_temps = 70 sec, difficulté = 5%

            // Si ce temps est meilleur que le précédent, on met à jour le meilleur temps
            if (pilotes[i].temps_meilleur_tour == 0.0 || temps_tour < pilotes[i].temps_meilleur_tour) {
                pilotes[i].temps_meilleur_tour = temps_tour;
            }

            // Affichage du temps actuel du pilote
            printf("Pilote: %s - Temps du tour: %.3f secondes\n", pilotes[i].nom, temps_tour);
        }

        // Tri des pilotes par leur meilleur temps à chaque tour
        tri_pilotes(pilotes, type_session);

        // Affichage du classement après chaque tour
        afficher_resultats(pilotes, type_session);
    }
}

// Fonction de tri des pilotes par leur meilleur temps de tour (tri à bulles)
void tri_pilotes(Pilote pilotes[], char* type_session) {
    // Tri des pilotes par temps (méthode de tri à bulles)
    for (int i = 0; i < NB_PILOTES - 1; i++) {
        for (int j = 0; j < NB_PILOTES - i - 1; j++) {
            if (pilotes[j].temps_meilleur_tour > pilotes[j + 1].temps_meilleur_tour) {
                // Échange des pilotes pour trier par temps de tour croissant
                Pilote temp = pilotes[j];
                pilotes[j] = pilotes[j + 1];
                pilotes[j + 1] = temp;
            }
        }
    }
}

// Fonction d'affichage des résultats en temps réel
void afficher_resultats(Pilote pilotes[], char* type_session) {
    printf("\nClassement actuel (%s):\n", type_session);
    for (int i = 0; i < NB_PILOTES; i++) {
        printf("%d. Pilote: %s - Meilleur tour: %.3f secondes\n", i + 1, pilotes[i].nom, pilotes[i].temps_meilleur_tour);
    }
}

// Exemple d'utilisation
int main() {
    Pilote pilotes[NB_PILOTES] = {
        {"Pilote 1", 0.0},
        {"Pilote 2", 0.0},
        {"Pilote 3", 0.0},
        {"Pilote 4", 0.0},
        {"Pilote 5", 0.0},
        {"Pilote 6", 0.0},
        {"Pilote 7", 0.0},
        {"Pilote 8", 0.0},
        {"Pilote 9", 0.0},
        {"Pilote 10", 0.0}
    };

    simulations_essais_libres(pilotes, "Essais Libres");

    return 0;
}
