#ifndef CARREFOUR_H
#define CARREFOUR_H

#include <pthread.h>
#include <semaphore.h>

#include "vehicule.h"
#include "serveur_controleur.h"  // Incluez le fichier d'en-tête du serveur

#define NOMBRE_VOIES 6
#define NOMBRE_CARREFOURS 4

// Déclaration avancée de la structure ServeurControleur
struct ServeurControleur;

typedef struct {
    int id;
    pthread_mutex_t mutex;
    Vehicule* debut;
    Vehicule* fin;
} Voie;

typedef struct {
    int id;
    Voie voies[NOMBRE_VOIES];
    struct ServeurControleur* serveur;  // Notez que le serveur est une structure, pas un pointeur
} Carrefour;

void initialiserCarrefour(Carrefour* carrefour, int id, ServeurControleur* serveur);
void* comportementCarrefour(void* arg);
void ajouterVehicule(Carrefour* carrefour, Vehicule* vehicule, int voie);
Vehicule* retirerVehicule(Carrefour* carrefour, int voie);
void demanderItineraireAuServeur(ServeurControleur* serveur, Carrefour* carrefour, Vehicule* vehicule);

#endif  // CARREFOUR_H
