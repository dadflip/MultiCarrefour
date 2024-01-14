# Projet de Simulation de Carrefours

Ce projet simule le fonctionnement de plusieurs carrefours avec des voies de circulation, des véhicules, et un contrôleur central.

## Table des matières

- [Introduction](#introduction)
- [Fonctionnalités](#fonctionnalités)
- [Structure du Projet](#structure-du-projet)
- [Compilation et Exécution](#compilation-et-exécution)
- [Configuration](#configuration)
- [Auteurs](#auteur)
- [Licence](#licence)

## Introduction

Le projet vise à modéliser le comportement des véhicules aux carrefours, en tenant compte des priorités, des itinéraires, et du contrôle central.

## Fonctionnalités

- Simulation de plusieurs carrefours avec des voies de circulation.
- Attribution des voies aux véhicules en fonction de leur priorité.
- Calcul d'itinéraires pour les véhicules.
- Affichage en temps réel de la simulation.
- Gestion des files de messages pour la communication entre les carrefours.

## Structure du Projet

Le projet est structuré en plusieurs fichiers et modules :

- `main.c` : Le point d'entrée du programme.
- `serveur_controleur.h` et `serveur_controleur.c` : Le contrôleur central.
- `carrefour.h` et `carrefour.c` : La simulation du carrefour.
- `vehicule.h` et `vehicule.c` : La modélisation des véhicules.
- `request.h` et `request.c` : La définition des types de messages pour les demandes et les réponses.

## Compilation et Exécution

Pour compiler le projet, exécutez le Makefile avec la commande `make` **ou**
exécutez le script `build-and-run` **ou** utilisez le compilateur de votre choix. Par exemple, avec `gcc` :

```bash
gcc main.c serveur_controleur.c carrefour.c vehicule.c -o simulation_carrefours -pthread
```

## Configuration
Le fichier de configuration config.h permet d'ajuster certaines variables du projet, telles que le nombre de carrefours, de voies, etc.

## Contributeurs

- **Kaoura**
- **Evy**
- **Axel** - [Profil GitHub](https://github.com/Flamware)
- **Davide** - [Profil GitHub](https://github.com/dadflip)


