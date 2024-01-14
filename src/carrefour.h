#ifndef CARREFOUR_H
#define CARREFOUR_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/msg.h>
#include <pthread.h>
#include "request.h"

#define NOMBRE_VOIES 6
#define NOMBRE_CARREFOURS 4

// Déclaration anticipée de la structure Vehicule
typedef struct Vehicule Vehicule;

struct Vehicule {
    int id;
    int type;
    char *icon;
    int vitesse;
    int destination;
    Vehicule* suivant;
};

typedef struct {
    int id;
    pthread_mutex_t mutex;
    Vehicule* debut;
    Vehicule* fin;
} Voie;

typedef struct {
    int id;
    unsigned int seed;
    int msgQueueID;
    Voie voies[NOMBRE_VOIES];
    Vehicule* vehiculesLibres; // Liste des véhicules libérés
} Carrefour;

typedef struct {
    Carrefour* carrefour;
    int voie;
} LibererVoieArgs;

void initialiserVoie(Voie* voie, int id);
void initialiserCarrefour(Carrefour* carrefour, int id, int msgQueueID);
void ajouterVehicule(Carrefour* carrefour, Vehicule* vehicule, int voie);
Vehicule* retirerVehicule(Carrefour* carrefour, int voie);
void* liberervoie(void* arg);
void envoyerRequest(Carrefour* carrefour, Vehicule* vehicule);
void recevoirResponse(Carrefour* carrefour);
void* comportementCarrefour(void* arg);
const char* getEmoticon(int type);

#endif  // CARREFOUR_H
