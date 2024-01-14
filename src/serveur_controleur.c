#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "serveur_controleur.h"
#include "request.h"
#include <sys/msg.h>
#include <wchar.h>

#define REQUEST_TYPE 1  // Type de message pour les demandes
#define RESPONSE_TYPE 2  // Type de message pour les réponses

int ID[NOMBRE_CARREFOURS];
volatile int simulationActive = 1;
void arreterSimulation() {
    simulationActive = 0;
}
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex global pour protéger l'accès à la file de messages

// Définir une fonction pour obtenir la priorité en fonction du type du véhicule
int getPriorite(int type) {
    // Plus le type est proche de 0, plus la priorité est élevée
    return 3 - type;
}


void initialiserServeurControleur(ServeurControleur* serveur) {

    // Initialisez les carrefours
    pthread_t threads[NOMBRE_CARREFOURS];

    // Créez les files de messages pour chaque carrefour
    for (int i = 0; i < NOMBRE_CARREFOURS; i++) {
        int existingQueueID = msgget(MSG_KEY_THREAD1 + i, 0666);
        if (existingQueueID != -1) {
            if (msgctl(existingQueueID, IPC_RMID, NULL) == -1) {
                perror("Erreur lors de la suppression de la file de messages existante");
                exit(EXIT_FAILURE);
            }
        }
        serveur->msgQueueIDs[i] = msgget(MSG_KEY_THREAD1 + i, IPC_CREAT | IPC_EXCL | 0666);
        if (serveur->msgQueueIDs[i] == -1) {
            perror("Erreur lors de la création/obtention de la file de messages");
            exit(EXIT_FAILURE);
        }
    }

    // Initialisez les carrefours et lancez les threads
    for (int i = 0; i < NOMBRE_CARREFOURS; i++) {
        initialiserCarrefour(&serveur->carrefours[i], i + 1, serveur->msgQueueIDs[i]);
        pthread_create(&threads[i], NULL, comportementCarrefour, &serveur->carrefours[i]);
    }


    // Initialisez les mutex et conditions
    for (int i = 0; i < NOMBRE_CARREFOURS; i++) {
        pthread_mutex_init(&serveur->condVarMutex[i], NULL);
        pthread_cond_init(&serveur->condVars[i], NULL);
    }

    // Créez les threads pour le calcul des itinéraires
    pthread_t threadsCalculItineraire[NOMBRE_CARREFOURS];

    for (int i = 0; i < NOMBRE_CARREFOURS; i++) {
        CalculItineraireArgs* args = (CalculItineraireArgs*)malloc(sizeof(CalculItineraireArgs));
        args->serveur = serveur;
        args->carrefourID = i + 1;
        pthread_create(&threadsCalculItineraire[i], NULL, calculerItineraire, args);
    }
    simulationVehicules(serveur);

    // Attendez que tous les threads se terminent
    for (int i = 0; i < NOMBRE_CARREFOURS; i++) {
        pthread_join(threads[i], NULL);
    }

}

void* calculerItineraire(void* args) {
    CalculItineraireArgs* arguments = (CalculItineraireArgs*)args;
    ServeurControleur* serveur = arguments->serveur;
    int carrefourID = arguments->carrefourID;
    free(arguments);

    // Boucle infinie pour calculer les itinéraires
    for (;;) {
        // Attendre une demande
        Request request;
        initialiserRequest(&request, 0, 0, 0, 0);
        if (msgrcv(serveur->msgQueueIDs[carrefourID - 1], &request, sizeof(Request) - sizeof(long), REQUEST_TYPE, 0) ==
            -1) {
            perror("Erreur lors de la réception de la demande du carrefour");
        } else {
        }
        // Chercher le vehicule
        Vehicule *vehicule = malloc(sizeof(Vehicule));
        for (int i = 0; i < NOMBRE_VOIES; i++) {
            if (serveur->carrefours[carrefourID - 1].voies[i].debut != NULL) {
                if (serveur->carrefours[carrefourID - 1].voies[i].debut->id == request.vehicule) {
                    vehicule = serveur->carrefours[carrefourID - 1].voies[i].debut;
                    break;
                }
            }
        }

	// Calculer la priorité du véhicule
        int priorite = getPriorite(vehicule->type);

        int voie = assignerVoiePrioritaire(&serveur->carrefours[request.itineraire - 1], vehicule, priorite);
        if (voie == 0) {
            printf("voies pleines et n'a pas sur attendre\n");
        }

        Response response;
        initialiserResponse(&response, carrefourID, request.vehicule, voie);

        // Envoyer la réponse
        response.mtype = RESPONSE_TYPE;
        if (msgsnd(serveur->msgQueueIDs[carrefourID - 1], &response, sizeof(Response) - sizeof(long), 0) == -1) {
            perror("Erreur lors de l'envoi de la réponse au carrefour");
        } else {
        }
        // thread pour retirer le vehicule de la voie
        pthread_t thread;

        // arguments pour le thread
        LibererVoieArgs *args = (LibererVoieArgs *) malloc(sizeof(LibererVoieArgs));
        args->carrefour = &serveur->carrefours[request.itineraire - 1];
        args->voie = voie;
        if (pthread_create(&thread, NULL, liberervoie, args) != 0) {  // <-- Added opening brace here
            perror("Erreur lors de la création du thread");
            exit(EXIT_FAILURE);
        }
    }// <-- Added closing brace here

}


// Fonction pour assigner la voie en fonction de la priorité
int assignerVoiePrioritaire(Carrefour* destination, Vehicule* vehicule, int priorite) {
    // Utiliser une plage de voies basée sur la priorité (0 à priorite)
    int debutPlage = 3;  // Voies normales commencent à l'indice 3
    int finPlage = debutPlage + priorite;

    if (finPlage >= NOMBRE_VOIES) {
        finPlage = NOMBRE_VOIES - 1;
    }

    for (int i = debutPlage; i <= finPlage; i++) {
        // Vérifier si le mutex est verrouillé, sinon le verrouiller
        if (pthread_mutex_trylock(&destination->voies[i].mutex) == 0) {
            // Verrouiller le mutex et assigner le véhicule à la voie
            if (destination->voies[i].debut == NULL) {
                destination->voies[i].debut = vehicule;
                destination->voies[i].fin = vehicule;
            } else {
                destination->voies[i].fin->suivant = vehicule;
                destination->voies[i].fin = vehicule;
            }
            return i + 1;  // Ajouter 1 car les voies sont indexées à partir de 1
        }
    }

    // Si aucune voie n'est disponible dans la plage, assigner une voie normale
    return assignerVoie(destination, vehicule);
}

// Ancienne fonction assignerVoie (appelée par assignerVoiePrioritaire si nécessaire)
int assignerVoie(Carrefour* destination, Vehicule* vehicule) {
    int i;
    // Attendre quelques secondes pour que l'affichage soit plus lisible
    sleep(2);
    for (;;) {
        for (i = 3; i < NOMBRE_VOIES; i++) {
            // Vérifier si le mutex est verrouillé, sinon le verrouiller
            if (pthread_mutex_trylock(&destination->voies[i].mutex) == 0) {
                // Verrouiller le mutex et assigner le véhicule à la voie
                if (destination->voies[i].debut == NULL) {
                    destination->voies[i].debut = vehicule;
                    destination->voies[i].fin = vehicule;
                } else {
                    destination->voies[i].fin->suivant = vehicule;
                    destination->voies[i].fin = vehicule;
                }
                return i + 1;  // Ajouter 1 car les voies sont indexées à partir de 1
            }
            if (i == NOMBRE_VOIES - 1) {
                i = 2;
            }
        }
    }
}


void* simulationVehicules(void* args) {
    ServeurControleur* serveur = (ServeurControleur*)args;
    int i = 0;
    char *priorityicon;
    while (simulationActive) {
        i++;
        printf("⏳ Simulation en cours...%d sec\n", i);
        for (int i = 0; i < NOMBRE_CARREFOURS / 2; i++) {
            printf("\n");
            printf("\n");
            printf("---------------------------------------------------------------------------------------------------------------\n");
            Carrefour* carrefour = &serveur->carrefours[i * 2];

            if (i % 2 != 0) {
                printf("\n");
            }

            printf("📍Carrefour %d:                +                   📍Carrefour %d:\n", carrefour->id, carrefour[1].id);
            for (int j = 0; j < NOMBRE_VOIES; j++) {
                if (j == 3) {
			printf("\n");
                }
                const char* typeVoie = (j < 3) ? "⬆️ D" : "⬇️ A";
		

                printf("%s %d: ", typeVoie, carrefour->voies[j].id);
		

                if (carrefour->voies[j].debut) {

		    if(carrefour->voies[j].debut->type == 0){
			priorityicon = "🚨";
		    }else{
			priorityicon = " ";
		    }

                    if (carrefour->voies[j].debut->destination == carrefour->id) {
		
                        printf("vehicle %d 🚦%s ... 🗺️ trajet", carrefour->voies[j].debut->id, carrefour->voies[j].debut->icon);
                    } else {

                        printf("%s vehicle %d %s  -> 📍%d", priorityicon, carrefour->voies[j].debut->id, carrefour->voies[j].debut->icon, carrefour->voies[j].debut->destination);
                    }
                    if (carrefour->voies[j].debut->id < 10) {
                        printf(" ");
                    }
                } else {
                    printf("*🍃");
                }

                printf("                                        %s %d: ", typeVoie, carrefour[1].voies[j].id);

		

                if (carrefour[1].voies[j].debut) {

                    if (carrefour[1].voies[j].debut->destination == carrefour[1].id) {
                        printf("vehicle %d 🚦%s ... 🗺️ trajet", carrefour[1].voies[j].debut->id, carrefour[1].voies[j].debut->icon);
                    } else {
                        printf("vehicle %d %s  -> 📍%d", carrefour[1].voies[j].debut->id, carrefour[1].voies[j].debut->icon, carrefour[1].voies[j].debut->destination);
                    }
                    if (carrefour[1].voies[j].debut->id < 10) {
                        printf(" ");
                    }
                } else {
                    printf("*🍃");
                }
                printf("\n");
            }
		
	    


        }
	printf("---------------------------------------------------------------------------------------------------------------\n");

	for (int i = 0; i < NOMBRE_CARREFOURS; i++) {
	    Carrefour* carrefour = &serveur->carrefours[i];

	    // Appeler la fonction pour afficher les 20 derniers véhicules libérés
	    afficherDerniersVehiculesLiberes(carrefour);

	    // Appeler la fonction pour réinitialiser la liste des véhicules libérés si nécessaire
	    reinitialiserListeVehiculesLiberes(carrefour);
	}


        usleep(1000000);
        system("clear");
    }

    printf("Simulation terminée\n");

    return NULL;
}



// Fonction pour afficher les icônes des 20 derniers véhicules libérés
void afficherDerniersVehiculesLiberes(Carrefour* carrefour) {
    printf("\nVéhicules libérés de Carrefour %d:\n", carrefour->id);

    Vehicule* vehicule = carrefour->vehiculesLibres;
    int count = 0;

    // Parcours des véhicules libérés jusqu'à atteindre les 20 derniers ou la fin de la liste
    while (vehicule && count < 20) {
        printf("   - Vehicle %d %s  -> 📍%d\n", vehicule->id, vehicule->icon, vehicule->destination);
        vehicule = vehicule->suivant;
        count++;
    }

}

// Fonction pour réinitialiser la liste des véhicules libérés si la taille atteint 50
void reinitialiserListeVehiculesLiberes(Carrefour* carrefour) {
    int tailleMax = 50;

    // Vérifier la taille de la liste des véhicules libérés
    int tailleListe = 0;
    Vehicule* vehicule = carrefour->vehiculesLibres;
    while (vehicule) {
        tailleListe++;
        vehicule = vehicule->suivant;
    }

    // Si la taille dépasse la limite, réinitialiser la liste
    if (tailleListe >= tailleMax) {
        printf("\nRéinitialisation de la liste des véhicules libérés de Carrefour %d.\n", carrefour->id);
        while (carrefour->vehiculesLibres) {
            vehicule = carrefour->vehiculesLibres;
            carrefour->vehiculesLibres = vehicule->suivant;
            free(vehicule);
        }
    }
}
