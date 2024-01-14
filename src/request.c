// request.c
#include "request.h"

void initialiserRequest(Request* request, int carrefourID, int vehicule,int ID, int intineraire) {
    // Utiliser un type de message positif non nul
    request->mtype = REQUEST_TYPE;  // Utiliser REQUEST_TYPE au lieu de 0
    request->CarrefourID = carrefourID;
    request->vehicule = vehicule;
    request->ID = ID;
    request->itineraire = intineraire;
}

void initialiserResponse(Response* response, int carrefourID, int vehicule, int itineraire) {
    // Utiliser un type de message positif non nul
    response->mtype = RESPONSE_TYPE;  // Utiliser REQUEST_TYPE au lieu de 0
    response->CarrefourID = carrefourID;
    response->vehicule = vehicule;
    response->itineraire = itineraire;
}