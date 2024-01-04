// serveur_controleur.h
#ifndef SERVEUR_CONTROLEUR_H
#define SERVEUR_CONTROLEUR_H

#include "carrefour.h"
#include <stdio.h>
#include <sys/msg.h>

// Clés de la file de messages pour les threads
#define MSG_KEY_THREAD1 0x1234
#define MSG_KEY_THREAD2 0x5678
#define MSG_KEY_THREAD3 0x9ABC
#define MSG_KEY_THREAD4 0xDEFF

#define REQUEST_TYPE 1  // Type de message pour les demandes
#define RESPONSE_TYPE 2  // Type de message pour les réponses

// Structure pour représenter le contrôleur du serveur
typedef struct {
    Carrefour carrefours[NOMBRE_CARREFOURS];
    int msgQueueIDs[NOMBRE_CARREFOURS];
    pthread_mutex_t condVarMutex[NOMBRE_CARREFOURS];  // Mutex pour protéger l'accès à la variable conditionnelle
    pthread_cond_t condVars[NOMBRE_CARREFOURS];  // Variable conditionnelle par carrefour
} ServeurControleur;

// Prototypes des fonctions liées au serveur
int calculerItineraire(int carrefourID, int voitureID, ServeurControleur* serveur);
void initialiserServeurControleur(ServeurControleur* serveur);

#endif  // SERVEUR_CONTROLEUR_H
