#ifndef PROJECT_REQUEST_H
#define PROJECT_REQUEST_H

#define REQUEST_TYPE 1
#define RESPONSE_TYPE 2

typedef struct {
    long mtype; // Champ nécessaire pour msgsnd et msgrcv
    int ID;
    int CarrefourID;
    int vehicule;
    int itineraire;
} Request;

typedef struct {
    long mtype;
    int CarrefourID;
    int vehicule;
    int itineraire;
} Response;

void initialiserRequest(Request* request, int carrefourID, int vehicule, int ID, int itineraire);
void initialiserResponse(Response* response, int carrefourID, int vehicule, int itineraire);

#endif //PROJECT_REQUEST_H
