// carrefour.h
#ifndef CARREFOUR_H
#define CARREFOUR_H

#include <pthread.h>
#include "request.h"

#define NOMBRE_VOIES 3
#define NOMBRE_CARREFOURS 4

#define REQUEST_TYPE 1
#define RESPONSE_TYPE 2

// Structure représentant un véhicule
typedef struct {
    int id;
    int type;
    int vitesse;
    int destination;
    struct Vehicule* suivant;
} Vehicule;

// Structure représentant une voie d'un carrefour
typedef struct {
    int id;
    pthread_mutex_t mutex;
    Vehicule* debut;
    Vehicule* fin;
} Voie;

// Structure représentant un carrefour
typedef struct {
    int id;
    unsigned int seed;
    int msgQueueID;
    Voie voies[NOMBRE_VOIES];
} Carrefour;

// Prototypes des fonctions
void initialiserVoie(Voie* voie, int id);
void initialiserCarrefour(Carrefour* carrefour, int id, int msgQueueID);
void ajouterVehicule(Carrefour* carrefour, Vehicule* vehicule, int voie);
Vehicule* retirerVehicule(Carrefour* carrefour, int voie);
void envoyerRequest(Carrefour* carrefour, Vehicule* vehicule);
void recevoirResponse(Carrefour* carrefour);
void* comportementCarrefour(void* arg);

#endif  // CARREFOUR_H
