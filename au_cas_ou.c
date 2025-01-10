void afficher_resultats_en_temps_reel(Pilote pilotes[], int tour, const char *session) {
    printf("\033[H\033[J");
    printf("--- Session : %s | Tour : %d ---\n", session, tour);
    printf("Classement actuel :\n");
    for (int i = 0; i < 20; i++) {
        printf("%d. Pilote: %s | Meilleur temps: %.3f sec | Dernier tour: %.3f sec\n",
               i + 1, pilotes[i].nom, pilotes[i].temps_meilleur_tour, pilotes[i].dernier_temps_tour);
    }
}
