#ifndef CARREFOUR_H
#define CARREFOUR_H

#include <pthread.h>
#include <semaphore.h>

// Inclure le fichier d'en-tête des véhicules
#include "vehicule.h"
#include "serveur_controleur.h"

#define NOMBRE_VOIES 6
#define NOMBRE_CARREFOURS 4

// Structure pour représenter une voie
typedef struct {
    int id;
    pthread_mutex_t mutex;  // Mutex pour synchroniser l'accès à la file
    Vehicule* debut;
    Vehicule* fin;
} Voie;

// Structure pour représenter un carrefour
typedef struct {
    int id;
    Voie voies[NOMBRE_VOIES];
    ServeurControleur* serveur;
    // Ajoutez d'autres informations spécifiques au carrefour ici
} Carrefour;

// Prototypes des fonctions liées au comportement du carrefour
void initialiserCarrefour(Carrefour* carrefour, int id, ServeurControleur* serveur);
void* comportementCarrefour(void* arg);
void ajouterVehicule(Carrefour* carrefour, Vehicule* vehicule, int voie);
Vehicule* retirerVehicule(Carrefour* carrefour, int voie);
void demanderItineraireAuServeur(ServeurControleur* serveur, Vehicule* vehicule);

#endif  // CARREFOUR_H
