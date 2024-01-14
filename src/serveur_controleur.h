// serveur_controleur.h
#ifndef SERVEUR_CONTROLEUR_H
#define SERVEUR_CONTROLEUR_H

#include "carrefour.h"
#include <pthread.h>

#define MSG_KEY_THREAD1 0x1234

// Structure pour représenter le contrôleur du serveur
typedef struct {
    Carrefour carrefours[NOMBRE_CARREFOURS];
    int msgQueueIDs[NOMBRE_CARREFOURS];
    pthread_mutex_t condVarMutex[NOMBRE_CARREFOURS];
    pthread_cond_t condVars[NOMBRE_CARREFOURS];
} ServeurControleur;

// Structure pour représenter les arguments de calcul d'itinéraire
typedef struct {
    ServeurControleur* serveur;
    int carrefourID;
} CalculItineraireArgs;

// Prototypes des fonctions liées au serveur
void * calculerItineraire(void* args);
void initialiserServeurControleur(ServeurControleur* serveur);
int assignerVoiePrioritaire(Carrefour* destination, Vehicule* vehicule, int priorite);
int assignerVoie(Carrefour* destination, Vehicule* vehicule);
void libererVoie(Carrefour* destination, int voie);
void* simulationVehicules(void* args);
void arreterSimulation();
void* testprint(void* args);
void afficherDerniersVehiculesLiberes(Carrefour* carrefour);
void reinitialiserListeVehiculesLiberes(Carrefour* carrefour);

#endif  // SERVEUR_CONTROLEUR_H
