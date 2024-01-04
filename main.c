#include <pthread.h>
#include "serveur_controleur.h"
#include "carrefour.h"

int main() {


    // Initialiser les carrefours en leur passant le pointeur vers le serveur de contrôle
    ServeurControleur serveur;
    initialiserServeurControleur(&serveur);

    return 0;
}
