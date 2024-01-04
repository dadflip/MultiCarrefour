#ifndef SERVEUR_CONTROLEUR_H
#define SERVEUR_CONTROLEUR_H

#include "carrefour.h"  // Incluez le fichier d'en-tête du carrefour

typedef struct {
    Carrefour carrefours[NOMBRE_CARREFOURS];
    pthread_mutex_t mutex; // Mutex pour synchroniser l'accès au serveur
} ServeurControleur;

// Prototypes des fonctions liées au comportement du serveur de contrôle
void initialiserServeurControleur(ServeurControleur* serveur);
void demanderItineraireAuServeur(ServeurControleur* serveur, Carrefour* carrefour, Vehicule* vehicule);

#endif  // SERVEUR_CONTROLEUR_H
