#include "serveur_controleur.h"
#include "carrefour.h"
#include <stdio.h>

void initialiserServeurControleur(ServeurControleur* serveur) {
    // Initialiser chaque carrefour
    for (int i = 0; i < NOMBRE_CARREFOURS; i++) {
        initialiserCarrefour(&(serveur->carrefours[i]), i + 1,serveur);  // Carrefours numérotés de 1 à NOMBRE_CARREFOURS
    }

    // Initialisez d'autres champs spécifiques au serveur de contrôle si nécessaire
}

void* comportementServeurControleur(void* arg) {

}
int itineraireOptimal(int depart, int arrivee){
    if(depart == 1 && arrivee == 2){
        return 0;
    }
    return 0;
}
