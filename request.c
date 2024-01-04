// request.c
#include "request.h"

void initialiserRequest(Request* request, int carrefourID, int voitureID,int ID) {
    // Utiliser un type de message positif non nul
    request->mtype = REQUEST_TYPE;  // Utiliser REQUEST_TYPE au lieu de 0
    request->CarrefourID = carrefourID;
    request->VoitureID = voitureID;
    request->ID = ID;
}

void initialiserResponse(Response* response, int carrefourID, int voitureID, int itineraire) {
    // Utiliser un type de message positif non nul
    response->mtype = RESPONSE_TYPE;  // Utiliser REQUEST_TYPE au lieu de 0
    response->CarrefourID = carrefourID;
    response->VoitureID = voitureID;
    response->itineraire = itineraire;
}