#include "serveur_controleur.h"

int main() {


    // Initialiser les carrefours en leur passant le pointeur vers le serveur de contrôle
    ServeurControleur serveur;
    initialiserServeurControleur(&serveur);
    arreterSimulation();

    return 0;
}
