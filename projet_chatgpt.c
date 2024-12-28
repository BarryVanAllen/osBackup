/*
Tout d'abord : 
1.sudo apt update   (Pour installer les dernières maj de ubuntu)
2.sudo apt install libsdl2-dev libsdl2-ttf-dev   (pour installer les 2 libs si pas la)
3.gcc projet.c -o projet -lSDL2 -lSDL2_ttf (pour compiler et lier des 2 libs installé)

4. ./projet (pour le lancer)
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <unistd.h>

#define NB_PILOTES 20
#define NB_TOURS_FP1 45
#define NB_TOURS_FP2 45
#define NB_TOURS_FP3 45
#define NB_TOURS_QUALIFICATION 20
#define NB_TOURS_COURSE 53
#define LARGEUR_FENETRE 1024
#define HAUTEUR_FENETRE 768

// Structure pour représenter un pilote
typedef struct {
    char nom[50];
    float temps_tour_actuel;
    float meilleur_temps;
    int position_actuelle;
    char dernier_temps_str[50];
} Pilote;

// Structures globales pour la gestion SDL
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
} Context_SDL;

// Prototypes de fonctions
void initialiser_pilotes(Pilote pilotes[]);
float generer_temps_tour(float base_temps);
void simuler_tour(Pilote pilotes[], int nb_pilotes, int tour_actuel);
void afficher_resultats_dynamiques(Context_SDL* sdl, Pilote pilotes[], int nb_pilotes, int tour_actuel, const char* titre);
Context_SDL* initialiser_sdl(const char* titre);
void nettoyer_sdl(Context_SDL* sdl);
void trier_pilotes(Pilote pilotes[], int nb_pilotes);

int main() {
    srand(time(NULL));
    
    // Initialisation des pilotes
    Pilote pilotes[NB_PILOTES];
    initialiser_pilotes(pilotes);
    
    // Initialisation SDL
    Context_SDL* sdl = initialiser_sdl("Simulation Weekend Formule 1");
    if (!sdl) {
        return 1;
    }
    
    // Essais Libres 1
    printf("--- Essais Libres 1 ---\n");
    for (int tour = 0; tour < NB_TOURS_FP1; tour++) {
        simuler_tour(pilotes, NB_PILOTES, tour);
        trier_pilotes(pilotes, NB_PILOTES);
        afficher_resultats_dynamiques(sdl, pilotes, NB_PILOTES, tour, "Essais Libres 1");
        SDL_Delay(2000);  // Pause de 2 secondes entre chaque tour
    }
    
    SDL_Delay(3000);  // Pause entre les sessions
    
    // Essais Libres 2
    printf("--- Essais Libres 2 ---\n");
    for (int tour = 0; tour < NB_TOURS_FP2; tour++) {
        simuler_tour(pilotes, NB_PILOTES, tour);
        trier_pilotes(pilotes, NB_PILOTES);
        afficher_resultats_dynamiques(sdl, pilotes, NB_PILOTES, tour, "Essais Libres 2");
        SDL_Delay(2000);  // Pause de 2 secondes entre chaque tour
    }
    
    SDL_Delay(3000);  // Pause entre les sessions
    
    // Essais Libres 3
    printf("--- Essais Libres 3 ---\n");
    for (int tour = 0; tour < NB_TOURS_FP3; tour++) {
        simuler_tour(pilotes, NB_PILOTES, tour);
        trier_pilotes(pilotes, NB_PILOTES);
        afficher_resultats_dynamiques(sdl, pilotes, NB_PILOTES, tour, "Essais Libres 3");
        SDL_Delay(2000);  // Pause de 2 secondes entre chaque tour
    }
    
    SDL_Delay(3000);  // Pause entre les sessions
    
    // Qualifications
    printf("--- Qualifications ---\n");
    for (int tour = 0; tour < NB_TOURS_QUALIFICATION; tour++) {
        simuler_tour(pilotes, NB_PILOTES, tour);
        trier_pilotes(pilotes, NB_PILOTES);
        afficher_resultats_dynamiques(sdl, pilotes, NB_PILOTES, tour, "Qualifications");
        SDL_Delay(2000);  // Pause de 2 secondes entre chaque tour
    }
    
    SDL_Delay(3000);  // Pause entre les sessions
    
    // Course
    printf("--- Course ---\n");
    for (int tour = 0; tour < NB_TOURS_COURSE; tour++) {
        simuler_tour(pilotes, NB_PILOTES, tour);
        trier_pilotes(pilotes, NB_PILOTES);
        afficher_resultats_dynamiques(sdl, pilotes, NB_PILOTES, tour, "Course Finale");
        SDL_Delay(2000);  // Pause de 2 secondes entre chaque tour
    }
    
    // Nettoyage
    nettoyer_sdl(sdl);
    free(sdl);
    
    return 0;
}

void initialiser_pilotes(Pilote pilotes[]) {
    const char* noms_pilotes[] = {
        "Verstappen", "Hamilton", "Pérez", "Russell", "Sainz",
        "Leclerc", "Norris", "Piastri", "Ocon", "Gasly",
        "Alonso", "Stroll", "Bottas", "Zhou", "Magnussen",
        "Sargeant", "Ricciardo", "De Vries", "Tsunoda", "Sargent"
    };
    
    for (int i = 0; i < NB_PILOTES; i++) {
        strcpy(pilotes[i].nom, noms_pilotes[i]);
        pilotes[i].temps_tour_actuel = 0.0;
        pilotes[i].meilleur_temps = INFINITY;
        pilotes[i].position_actuelle = i + 1;
        strcpy(pilotes[i].dernier_temps_str, "N/A");
    }
}

float generer_temps_tour(float base_temps) {
    // Génération de temps avec plus de variabilité
    float variation = ((float)rand() / RAND_MAX * 2.0 - 1.0) * 2.0;  // -2 à +2 secondes
    return base_temps + variation;
}

void simuler_tour(Pilote pilotes[], int nb_pilotes, int tour_actuel) {
    float base_temps = 80.0;  // Temps de base d'un tour
    
    // Simulation des temps de tour
    for (int i = 0; i < nb_pilotes; i++) {
        pilotes[i].temps_tour_actuel = generer_temps_tour(base_temps);
        
        // Mise à jour du meilleur temps
        if (pilotes[i].temps_tour_actuel < pilotes[i].meilleur_temps) {
            pilotes[i].meilleur_temps = pilotes[i].temps_tour_actuel;
        }
        
        // Formater le temps pour l'affichage
        snprintf(pilotes[i].dernier_temps_str, sizeof(pilotes[i].dernier_temps_str), 
                 "Tour %d: %.3f s", tour_actuel + 1, pilotes[i].temps_tour_actuel);
    }
}

Context_SDL* initialiser_sdl(const char* titre) {
    Context_SDL* sdl = malloc(sizeof(Context_SDL));
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur d'initialisation SDL: %s\n", SDL_GetError());
        return NULL;
    }
    
    if (TTF_Init() == -1) {
        printf("Erreur d'initialisation SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return NULL;
    }
    
    sdl->window = SDL_CreateWindow(titre, 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        LARGEUR_FENETRE, HAUTEUR_FENETRE, 
        SDL_WINDOW_SHOWN
    );
    
    if (!sdl->window) {
        printf("Erreur de création de fenêtre: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }
    
    sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);
    if (!sdl->renderer) {
        printf("Erreur de création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(sdl->window);
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }
    
    sdl->font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 20);
    if (!sdl->font) {
        printf("Erreur de chargement de la police: %s\n", TTF_GetError());
        SDL_DestroyRenderer(sdl->renderer);
        SDL_DestroyWindow(sdl->window);
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }
    
    return sdl;
}

void nettoyer_sdl(Context_SDL* sdl) {
    if (sdl) {
        if (sdl->font) TTF_CloseFont(sdl->font);
        if (sdl->renderer) SDL_DestroyRenderer(sdl->renderer);
        if (sdl->window) SDL_DestroyWindow(sdl->window);
    }
    TTF_Quit();
    SDL_Quit();
}


void trier_pilotes(Pilote pilotes[], int nb_pilotes) {
    // Trier les pilotes en fonction de leur meilleur temps (ordre croissant)
    for (int i = 0; i < nb_pilotes - 1; i++) {
        for (int j = 0; j < nb_pilotes - i - 1; j++) {
            if (pilotes[j].meilleur_temps > pilotes[j + 1].meilleur_temps) {
                Pilote temp = pilotes[j];
                pilotes[j] = pilotes[j + 1];
                pilotes[j + 1] = temp;
            }
        }
    }
}

void afficher_resultats_dynamiques(Context_SDL* sdl, Pilote pilotes[], int nb_pilotes, int tour_actuel, const char* titre) {
    // Nettoyer l'écran
    SDL_SetRenderDrawColor(sdl->renderer, 255, 255, 255, 255);
    SDL_RenderClear(sdl->renderer);
    
    // Couleurs
    SDL_Color couleur_titre = {0, 0, 0, 255};
    SDL_Color couleur_texte = {50, 50, 50, 255};
    SDL_Color couleur_meilleur_temps = {0, 128, 0, 255};
    
    // Afficher le titre de la session
    char titre_complet[100];
    snprintf(titre_complet, sizeof(titre_complet), "%s - Tour %d", titre, tour_actuel + 1);
    SDL_Surface* surface_titre = TTF_RenderText_Blended(sdl->font, titre_complet, couleur_titre);
    SDL_Texture* texture_titre = SDL_CreateTextureFromSurface(sdl->renderer, surface_titre);
    
    SDL_Rect position_titre;
    position_titre.x = 50;
    position_titre.y = 20;
    SDL_QueryTexture(texture_titre, NULL, NULL, &position_titre.w, &position_titre.h);
    SDL_RenderCopy(sdl->renderer, texture_titre, NULL, &position_titre);
    
    SDL_FreeSurface(surface_titre);
    SDL_DestroyTexture(texture_titre);
    
    // Afficher les résultats des pilotes
    for (int i = 0; i < nb_pilotes; i++) {
        char ligne[200];
        snprintf(ligne, sizeof(ligne), "%d. %s : %s (Meilleur: %.3f s)", 
                 i + 1, pilotes[i].nom, pilotes[i].dernier_temps_str, pilotes[i].meilleur_temps);
        
        SDL_Surface* surface;
        SDL_Color couleur_ligne = (pilotes[i].temps_tour_actuel == pilotes[i].meilleur_temps) 
                                  ? couleur_meilleur_temps : couleur_texte;
        surface = TTF_RenderText_Blended(sdl->font, ligne, couleur_ligne);
        
        SDL_Texture* texture = SDL_CreateTextureFromSurface(sdl->renderer, surface);
        
        SDL_Rect position;
        position.x = 50;
        position.y = 100 + (i * 30);
        SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
        
        SDL_RenderCopy(sdl->renderer, texture, NULL, &position);
        
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
    
    // Mettre à jour l'affichage
    SDL_RenderPresent(sdl->renderer);
}
