#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/msg.h>
#include "serveur_controleur.h"
#include "request.h"

#define REQUEST_TYPE 1  // Type de message pour les demandes
#define RESPONSE_TYPE 2  // Type de message pour les réponses

int ID[NOMBRE_CARREFOURS];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex global pour protéger l'accès à la file de messages

void initialiserServeurControleur(ServeurControleur* serveur) {

    // Initialisez les carrefours
    pthread_t threads[NOMBRE_CARREFOURS];

    // Créez les files de messages pour chaque carrefour
    for (int i = 0; i < NOMBRE_CARREFOURS; i++) {

        // Vérifiez si la file de messages existe
        int existingQueueID = msgget(MSG_KEY_THREAD1 + i, 0666);
        if (existingQueueID != -1) {
            // La file de messages existe, supprimez-la
            if (msgctl(existingQueueID, IPC_RMID, NULL) == -1) {
                perror("Erreur lors de la suppression de la file de messages existante");
                exit(EXIT_FAILURE);
            }
        }

        // Créez une nouvelle file de messages
        serveur->msgQueueIDs[i] = msgget(MSG_KEY_THREAD1 + i, IPC_CREAT | IPC_EXCL | 0666);
        if (serveur->msgQueueIDs[i] == -1) {
            perror("Erreur lors de la création/obtention de la file de messages");
            printf("errno: %d\n", errno);
            exit(EXIT_FAILURE);
        }
        printf("File de messages créée avec succès avec l'ID %d pour le carrefour %d\n", serveur->msgQueueIDs[i],
               i + 1);

    }



    // Initialisez les carrefours et lancez les threads
    for (int i = 0; i < NOMBRE_CARREFOURS; i++) {
        initialiserCarrefour(&serveur->carrefours[i], i + 1, serveur->msgQueueIDs[i]);
        pthread_create(&threads[i], NULL, comportementCarrefour, &serveur->carrefours[i]);
    }

    while (1) {
        for (int i = 0; i < NOMBRE_CARREFOURS; i++) {
            // Réception des messages et traitement pour chaque carrefour
            Request request;
            initialiserRequest(&request, 0, 0, 0);
            if (msgrcv(serveur->msgQueueIDs[i], &request, sizeof(Request) - sizeof(long), REQUEST_TYPE, IPC_NOWAIT) ==
                -1) {
                if (errno != ENOMSG) {
                    perror("Erreur lors de la réception de la demande du carrefour");
                    exit(EXIT_FAILURE);
                }
            } else {
                calculerItineraire(request.CarrefourID, request.VoitureID, serveur);
            }
        }
    }
}

int calculerItineraire(int carrefourID, int voitureID, ServeurControleur* serveur) {
    // ... Votre logique de calcul de l'itinéraire ici ...

    // Créez une réponse et initialisez ses champs
    int itineraire = rand() % 4 + 1;

    Response response;
    initialiserResponse(&response, carrefourID, voitureID, itineraire);

    // Envoyer la réponse à la file de messages
    if (msgsnd(serveur->msgQueueIDs[carrefourID - 1], &response, sizeof(response) - sizeof(long), 0) == -1) {
        perror("Erreur lors de l'envoi de la réponse au carrefour");
        exit(EXIT_FAILURE);
    }
    printf("Réponse envoyée par le serveur au carrefour %d pour le véhicule %d, itinéraire : %d\n", carrefourID, voitureID, itineraire);

    return 0;
}
