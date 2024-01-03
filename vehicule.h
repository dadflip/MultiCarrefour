#ifndef VEHICULE_H
#define VEHICULE_H

#include <pthread.h>

// Enumération pour les types de véhicules

typedef enum {
    VOITURE,
    CAMION,
    POLICE,
    POMPIER
} TypeVehicule;

typedef struct Vehicule {
    int id;
    TypeVehicule type;
    int vitesse;
    int destination;  // Identifiant du carrefour de destination
    struct Vehicule* suivant;  // Pointeur vers le véhicule suivant dans la file
    int voie;
} Vehicule;

// Prototypes des fonctions liées au comportement du véhicule
void initialiserVehicule(Vehicule* vehicule, int id, TypeVehicule type, int vitesse, int voie);
void* comportementVehicule(void* arg);
void afficherIineaire();

#endif  // VEHICULE_H
