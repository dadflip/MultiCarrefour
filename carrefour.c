#include "carrefour.h"
#include "vehicule.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "serveur_controleur.h"

// Fonction pour initialiser une voie
void initialiserVoie(Voie* voie, int id) {
    voie->id = id;
    pthread_mutex_init(&(voie->mutex), NULL);
    voie->debut = NULL;
    voie->fin = NULL;
}

// Fonction pour initialiser le carrefour
void initialiserCarrefour(Carrefour* carrefour, int id, ServeurControleur* serveur) {
    carrefour->id = id;

    // Initialiser les voies du carrefour
    for (int i = 0; i < NOMBRE_VOIES; i++) {
        initialiserVoie(&(carrefour->voies[i]), i + 1);
    }
}

// Fonction pour ajouter un véhicule à une voie du carrefour
void ajouterVehicule(Carrefour* carrefour, Vehicule* vehicule, int voie){
    pthread_mutex_lock(&(carrefour->voies[voie - 1].mutex));

    // Ajouter le véhicule à la fin de la file de la voie spécifiée
    vehicule->suivant = NULL;
    if (carrefour->voies[voie - 1].debut == NULL) {
        carrefour->voies[voie - 1].debut = vehicule;
        carrefour->voies[voie - 1].fin = vehicule;
    } else {
        carrefour->voies[voie - 1].fin->suivant = vehicule;
        carrefour->voies[voie - 1].fin = vehicule;
    }

    pthread_mutex_unlock(&(carrefour->voies[voie - 1].mutex));
}

// Fonction pour retirer un véhicule d'une voie du carrefour
Vehicule* retirerVehicule(Carrefour* carrefour, int voie) {
    pthread_mutex_lock(&(carrefour->voies[voie - 1].mutex));

    Vehicule* vehicule = NULL;
    if (carrefour->voies[voie - 1].debut != NULL) {
        vehicule = carrefour->voies[voie - 1].debut;
        carrefour->voies[voie - 1].debut = vehicule->suivant;
        if (carrefour->voies[voie - 1].debut == NULL) {
            carrefour->voies[voie - 1].fin = NULL;
        }
    }

    pthread_mutex_unlock(&(carrefour->voies[voie - 1].mutex));

    return vehicule;
}

// Fonction pour le comportement du carrefour
void* comportementCarrefour(void* arg) {
    Carrefour* carrefour = (Carrefour*)arg;

    srand((unsigned int)time(NULL));

    while (1) {
        // Générer un nombre aléatoire de véhicules entre 1 et 3
        int nombreVehicule = rand() % 3 + 1;

        // Générer des véhicules aléatoires
        for (int i = 0; i < nombreVehicule; i++) {
            Vehicule* vehicule = (Vehicule*)malloc(sizeof(Vehicule));
            if (vehicule != NULL) {
                vehicule->id = i + 1;
                vehicule->type = rand() % 4;
                vehicule->vitesse = rand() % 100 + 1;
                vehicule->destination = rand() % NOMBRE_CARREFOURS + 1;

                // Ajouter le véhicule à une voie aléatoire du carrefour
                int voieAleatoire = rand() % NOMBRE_VOIES + 1 -3;
                ajouterVehicule(carrefour, vehicule, voieAleatoire);
            }
        }

        // Attendre quelques secondes avant de générer de nouveaux véhicules
        sleep(5);
    }

    pthread_exit(NULL);
}

void demanderItineraireAuServeur(ServeurControleur* serveur, Carrefour* carrefour, Vehicule* vehicule) {
    // Appeler la fonction du serveur pour demander l'itinéraire
    demanderItineraireAuServeur(&(carrefour->serveur), carrefour, vehicule);
}