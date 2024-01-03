#include "vehicule.h"
#include <stdio.h>
#include <stdlib.h>

void initialiserVehicule(Vehicule* vehicule, int id, TypeVehicule type, int vitesse, int voie) {
    vehicule->id = id;
    vehicule->type = type;
    vehicule->vitesse = vitesse;
    vehicule->voie = 0;
    // Initialisez d'autres champs spécifiques au véhicule si nécessaire
}

void* comportementVehicule(void* arg) {
    Vehicule* vehicule = (Vehicule*)arg;
    // Logique du comportement du véhicule
    printf("Véhicule %d de type %d avec une vitesse de %d.\n", vehicule->id, vehicule->type, vehicule->vitesse);
    // Ajoutez le comportement spécifique du véhicule ici
    pthread_exit(NULL);
}
