#include "carrefour.h"  // Assurez-vous que le fichier d'en-tête du carrefour est inclus

void initialiserServeurControleur(ServeurControleur* serveur) {
    // Code d'initialisation ici
    for (int i = 0; i < NOMBRE_CARREFOURS; i++) {
        initialiserCarrefour(&(serveur->carrefours[i]), i + 1, serveur);
    }
}
