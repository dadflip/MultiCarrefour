#ifndef SERVEUR_CONTROLEUR_H
#define SERVEUR_CONTROLEUR_H

// Déclaration avancée de la structure Carrefour
struct Carrefour;

// Structure pour représenter le serveur de contrôle
typedef struct {
    struct Carrefour* carrefours[4];  // Tableau des carrefours interconnectés
    // Ajoutez d'autres informations spécifiques au serveur de contrôle ici
} ServeurControleur;

// Prototypes des fonctions liées au comportement du serveur de contrôle
void initialiserServeurControleur(ServeurControleur* serveur);

#endif  // SERVEUR_CONTROLEUR_H
