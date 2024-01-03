#include <pthread.h>
#include "serveur_controleur.c"
#include "carrefour.h"
#include "vehicule.h"

int main() {
    // Initialiser le serveur de contrôle
    ServeurControleur serveur;
    initialiserServeurControleur(&serveur);

    // Initialiser les carrefours en leur passant le pointeur vers le serveur de contrôle
    Carrefour carrefour1, carrefour2, carrefour3, carrefour4;
    initialiserCarrefour(&carrefour1, 1, &serveur);
    initialiserCarrefour(&carrefour2, 2, &serveur);
    initialiserCarrefour(&carrefour3, 3, &serveur);
    initialiserCarrefour(&carrefour4, 4, &serveur);

    // Créer des threads pour les carrefours
    pthread_t threadCarrefour1, threadCarrefour2, threadCarrefour3, threadCarrefour4;

    pthread_create(&threadCarrefour1, NULL, comportementCarrefour, (void*)&carrefour1);
    pthread_create(&threadCarrefour2, NULL, comportementCarrefour, (void*)&carrefour2);
    pthread_create(&threadCarrefour3, NULL, comportementCarrefour, (void*)&carrefour3);
    pthread_create(&threadCarrefour4, NULL, comportementCarrefour, (void*)&carrefour4);

    // Attendre la fin des threads
    pthread_join(threadCarrefour1, NULL);
    pthread_join(threadCarrefour2, NULL);
    pthread_join(threadCarrefour3, NULL);
    pthread_join(threadCarrefour4, NULL);

    return 0;
}
