#include "carrefour.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/msg.h>
#include "request.h"


static int lastRequestID = 1;
static pthread_mutex_t lastRequestIDMutex = PTHREAD_MUTEX_INITIALIZER;
static int lastVehicleID = 1;
static pthread_mutex_t lastVehicleIDMutex = PTHREAD_MUTEX_INITIALIZER;

// Fonction pour initialiser une voie
void initialiserVoie(Voie* voie, int id) {
    voie->id = id;
    pthread_mutex_init(&(voie->mutex), NULL);
    voie->debut = NULL;
    voie->fin = NULL;
}
void initialiserCarrefour(Carrefour* carrefour, int id, int msgQueueID) {
    carrefour->id = id;

    // Utilisez le temps actuel comme graine
    carrefour->seed = (unsigned int)time(NULL) + id;
    carrefour->msgQueueID = msgQueueID;

    for (int i = 0; i < NOMBRE_VOIES; i++) {
        initialiserVoie(&(carrefour->voies[i]), i + 1);
    }
}


// Fonction pour initialiser le carrefour


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

void envoyerRequest(Carrefour* carrefour, Vehicule* vehicule) {
    Request request;

    // Lock before incrementing the shared variable
    pthread_mutex_lock(&lastRequestIDMutex);
    request.ID = lastRequestID++;
    pthread_mutex_unlock(&lastRequestIDMutex);

    initialiserRequest(&request, carrefour->id, vehicule->id, request.ID);

    // Envoyer la demande
    if (msgsnd(carrefour->msgQueueID, &request, sizeof(Request) - sizeof(long), 0) == -1) {
        perror("Erreur lors de l'envoi de la demande au serveur");
    }

}


void recevoirResponse(Carrefour* carrefour) {
    Response response;
    initialiserResponse(&response, 0, 0, 0);
    if (msgrcv(carrefour->msgQueueID, &response, sizeof(Response) - sizeof(long), RESPONSE_TYPE, 0) == -1) {
        perror("Erreur lors de la réception de la réponse du serveur");
    } else {
        printf("Réponse reçue par le carrefour %d pour le véhicule %d.\n", response.CarrefourID, response.VoitureID);
    }
}



void* comportementCarrefour(void* arg) {
    Carrefour* carrefour = (Carrefour*)arg;
    unsigned int seed = carrefour->seed;

    srand((unsigned int)time(NULL));
    for (;;) {
        int nombreVehicule = rand_r(&seed) % 3 + 1;

        for (int i = 0; i < nombreVehicule; i++) {
            Vehicule* vehicule = (Vehicule*)malloc(sizeof(Vehicule));
            if (vehicule != NULL) {
                pthread_mutex_lock(&lastVehicleIDMutex);
                vehicule->id = lastVehicleID++;
                pthread_mutex_unlock(&lastVehicleIDMutex);
                vehicule->type = rand_r(&seed) % 4;
                vehicule->vitesse = rand_r(&seed) % 100 + 1;
                vehicule->destination = rand_r(&seed) % NOMBRE_CARREFOURS + 1;

                // Ajouter le véhicule à une voie aléatoire entre 1 et NOMBRE_VOIES
                int voieAleatoire = rand_r(&seed) % NOMBRE_VOIES + 1;
                ajouterVehicule(carrefour, vehicule, voieAleatoire);

                envoyerRequest(carrefour, vehicule);

                recevoirResponse(carrefour);
            }
        }
        sleep(2);
        printf("Nombre de véhicules créés par le carrefour %d : %d\n", carrefour->id, nombreVehicule);


        sleep(8);
    }

    pthread_exit(NULL);
}