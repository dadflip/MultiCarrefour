// request.h
#ifndef PROJECT_REQUEST_H
#define PROJECT_REQUEST_H
#define REQUEST_TYPE 1
#define RESPONSE_TYPE 2

typedef struct {
    long mtype; // Champ nécessaire pour msgsnd et msgrcv
    int ID;
    int CarrefourID;
    int VoitureID;
    int messageType;  // 0 pour demande, 1 pour réponse, par exemple
    // ... autres champs ...
} Request;

typedef struct {
    long mtype;
    int CarrefourID;
    int VoitureID;
    int itineraire;
} Response;

void initialiserRequest(Request* request, int carrefourID, int voitureID, int ID);
void initialiserResponse(Response* response, int carrefourID, int voitureID, int itineraire);

#endif //PROJECT_REQUEST_H
