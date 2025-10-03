# Protocole CBP (Consultation Booking Protocol)

## Description
Implémentation du protocole de communication entre le client de réservation et le serveur, incluant la logique métier et la gestion de la base de données.

## Objectif
Traiter les requêtes des clients pour la gestion des consultations médicales : authentification, recherche, réservation.

## Fichiers
- **`cbp.h`** : Déclarations des fonctions du protocole
- **`cbp.cpp`** : Implémentation complète du protocole CBP

## Commandes du Protocole

### LOGIN
- **Requête** : `LOGIN#nom#prénom#numeroPatient#nouveauPatient`
- **Réponse** : `LOGIN#ok#patientId` ou `LOGIN#ko#erreur`
- **Action** : Authentification ou création d'un nouveau patient

### LOGOUT  
- **Requête** : `LOGOUT`
- **Réponse** : `LOGOUT#ok#`
- **Action** : Déconnexion du patient

### GET_SPECIALTIES
- **Requête** : `GET_SPECIALTIES`
- **Réponse** : `GET_SPECIALTIES#ok##id1#nom1|id2#nom2|...`
- **Action** : Récupération de toutes les spécialités médicales

### GET_DOCTORS
- **Requête** : `GET_DOCTORS` 
- **Réponse** : `GET_DOCTORS#ok##id1#nomComplet1#spécialité1|...`
- **Action** : Récupération de tous les médecins

### SEARCH_CONSULTATIONS
- **Requête** : `SEARCH_CONSULTATIONS#spécialité#médecin#dateDebut#dateFin`
- **Réponse** : `SEARCH_CONSULTATIONS#ok##id1#médecin1#spécialité1#date1#heure1|...`
- **Action** : Recherche des consultations disponibles selon critères

### BOOK_CONSULTATION
- **Requête** : `BOOK_CONSULTATION#consultationId#raison`
- **Réponse** : `BOOK_CONSULTATION#ok#message` ou `BOOK_CONSULTATION#ko#erreur`
- **Action** : Réservation d'une consultation

## Fonctionnalités techniques

### Gestion des sessions
- **Structure CLIENT_INFO** : Mémorisation des clients connectés
- **Fonctions** : `estPresent()`, `ajoute()`, `retire()`
- **Thread-safety** : Mutex pour accès concurrent

### Base de données
- **Connexion MySQL** : `connecterBD()`, `deconnecterBD()`
- **Exécution requêtes** : `executerRequeteBD()`
- **Thread-safety** : Mutex pour BD partagée

### Sécurité
- **Authentification** : Vérification pour chaque commande
- **Validation** : Contrôle des paramètres
- **Isolation** : Chaque client dans sa session

## Administration (Préparé pour Java)
- **`CBP_GetPatientsConnectes()`** : Liste des patients connectés avec IP
- **Format** : `patientId#nom#prénom#ip|...`

## Architecture
```
Client Qt ←→ [TCP] ←→ Serveur ←→ [CBP] ←→ MySQL
```

## Thread-Safety
- **mutexClients** : Protection liste des clients
- **mutexBD** : Protection accès base de données
- **Fonctions atomiques** : Toutes les opérations protégées