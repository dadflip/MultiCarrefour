#include "carrefour.h"


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

Vehicule* retirerVehicule(Carrefour* carrefour, int voie){
    pthread_mutex_lock(&(carrefour->voies[voie - 1].mutex));

    // Retirer le véhicule du début de la file de la voie spécifiée
    Vehicule* vehicule = carrefour->voies[voie - 1].debut;
    if (vehicule != NULL) {
        carrefour->voies[voie - 1].debut = vehicule->suivant;
        vehicule->suivant = NULL;
	carrefour->vehiculesLibres = vehicule;
    }

    pthread_mutex_unlock(&(carrefour->voies[voie - 1].mutex));
    return vehicule;
}

void* liberervoie(void* arg){
    LibererVoieArgs* threadArgs = (LibererVoieArgs*)arg;
    Carrefour* carrefour = threadArgs->carrefour;
    int voie = threadArgs->voie;

    // Simuler le temps de passage du véhicule
    sleep(10);

    // retirer le véhicule de la voie
    pthread_mutex_unlock(&(carrefour->voies[voie - 1].mutex));
    carrefour->voies[voie - 1].debut = NULL;
    carrefour->voies[voie - 1].fin = NULL;

    pthread_exit(NULL);
}


void envoyerRequest(Carrefour* carrefour, Vehicule* vehicule) {
    Request request;
    // Lock before incrementing the shared variable
    pthread_mutex_lock(&lastRequestIDMutex);
    request.ID = lastRequestID++;
    pthread_mutex_unlock(&lastRequestIDMutex);
    initialiserRequest(&request, carrefour->id, vehicule->id, request.ID, vehicule->destination);
    request.mtype = REQUEST_TYPE;
    // Envoyer la demande
    if (msgsnd(carrefour->msgQueueID, &request, sizeof(Request) - sizeof(long), 0) == -1) {
        perror("Erreur lors de l'envoi de la demande au serveur");
    }

}


void recevoirResponse(Carrefour* carrefour) {
    Response response;
    // Recevoir la réponse
    if (msgrcv(carrefour->msgQueueID, &response, sizeof(Response) - sizeof(long), RESPONSE_TYPE, 0) == -1) {
        perror("Erreur lors de la réception de la réponse du serveur");
    }
}
void* comportementCarrefour(void* arg) {
    Carrefour* carrefour = (Carrefour*)arg;
    unsigned int seed = carrefour->seed;

    srand((unsigned int)time(NULL));
    //initialiser les mutex pour les voies et les debloquer
    for (int i = 0; i < NOMBRE_VOIES; i++) {
        pthread_mutex_init(&(carrefour->voies[i].mutex), NULL);
    }
    for (;;) {
        int nombreVehicule = rand_r(&seed) % 3 + 1;

        for (int i = 0; i < nombreVehicule; i++) {
            Vehicule* vehicule = (Vehicule*)malloc(sizeof(Vehicule));
            if (vehicule != NULL) {
                pthread_mutex_lock(&lastVehicleIDMutex);
                vehicule->id = lastVehicleID++;
                pthread_mutex_unlock(&lastVehicleIDMutex);
                vehicule->type = rand_r(&seed) % 4;
		vehicule->icon = getEmoticon(vehicule->type);
                vehicule->vitesse = rand_r(&seed) % 100 + 1;
                // La destination doit être différente du carrefour actuel
                do {
                    vehicule->destination = rand_r(&seed) % NOMBRE_CARREFOURS + 1;
                } while (vehicule->destination == carrefour->id);
                // Ajouter le véhicule à une voie aléatoire entre 1 et 3
                int voieAleatoire = rand_r(&seed) % 3 + 1;
                ajouterVehicule(carrefour, vehicule, voieAleatoire);

                envoyerRequest(carrefour, vehicule);
                recevoirResponse(carrefour);
                retirerVehicule(carrefour, voieAleatoire);
            }
        }
    }

}

// Fonction pour définir un émoticône en fonction du type de véhicule
const char* getEmoticon(int type) {
    // Initialiser le générateur de nombres aléatoires avec une graine basée sur le temps
    srand(time(NULL));

    // Tableau d'émoticônes pour chaque type
	const char* emoticons[][5] = {
	    {"🚓", "🚑", "🚒"}, // Émoticônes pour le type 0 (priorité 0)
	    {"🚕", "🚗", "🚙"}, // Émoticônes pour le type 1 (priorité 1)
	    {"🏍️", "🛵", "🚲", "🛴"}, // Émoticônes pour le type 2 (priorité 2)
	    {"🚚", "🚛", "🚌"}    // Émoticônes pour le type 3 (priorité 3)
	};

	// Nombre d'émoticônes associées à chaque type
	const int emoticonCount[] = {3, 3, 3, 3};


    // Vérifier si le type est valide
    if (type >= 0 && type < sizeof(emoticons) / sizeof(emoticons[0])) {
        // Choisir un indice aléatoire pour l'émoticône
        int randomIndex = rand() % emoticonCount[type];

        // Retourner l'émoticône correspondante
        return emoticons[type][randomIndex];
    } else {
        // Retourner une émoticône par défaut pour les types invalides
        return "❓";
    }
}