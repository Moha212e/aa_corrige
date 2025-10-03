# Client de Réservation Qt

## Description
Application graphique Qt permettant aux patients de réserver des consultations médicales via une interface conviviale.

## Objectif
Fournir une borne d'accueil interactive pour que les patients puissent s'identifier, rechercher et réserver des consultations.

## Fichiers principaux
- **`main.cpp`** : Point d'entrée de l'application Qt
- **`mainwindowclientconsultationbooker.h/cpp`** : Logique de l'interface
- **`ui_mainwindowclientconsultationbooker.h`** : Interface utilisateur générée
- **`moc_*`** : Fichiers générés par Qt (Meta-Object Compiler)

## Fonctionnalités utilisateur

### 🔐 Authentification
- **Champs** : Nom, Prénom, Numéro Patient
- **Nouveau patient** : Checkbox pour création de compte
- **Boutons** : Login/Logout

### 🔍 Recherche de consultations
- **Critères** : Spécialité, Médecin, Plage de dates
- **Affichage** : Table des consultations disponibles
- **Tri** : Par date et heure

### 📅 Réservation
- **Sélection** : Consultation dans la table
- **Raison** : Saisie via boîte de dialogue
- **Confirmation** : Message de succès/échec

## Interface utilisateur

### Zone d'identification
- Champs texte : Nom, Prénom
- Spin box : Numéro patient (1-99999)
- Checkbox : "Nouveau Patient"
- Boutons : Login (vert), Logout (standard)

### Zone de recherche
- ComboBox : Spécialités (chargées du serveur)
- ComboBox : Médecins (chargés du serveur)
- DateEdit : Date début/fin
- Bouton : "Lancer la recherche" (bleu)

### Zone des résultats
- TableWidget : Consultations trouvées
- Colonnes : ID, Spécialité, Médecin, Date, Heure
- Sélection : Une ligne à la fois
- Bouton : "Réserver" (saumon)

## États de l'interface

### Avant connexion
- ✅ Champs identification actifs
- ❌ Recherche/Réservation désactivées

### Après connexion réussie
- ❌ Champs identification verrouillés
- ✅ Recherche/Réservation activées
- ✅ Chargement automatique spécialités/médecins

## Communication réseau
- **Protocole** : CBP via TCP
- **Serveur par défaut** : 127.0.0.1:8080
- **Format messages** : `COMMANDE#param1#param2#...`
- **Gestion erreurs** : Boîtes de dialogue informatives

## Workflow utilisateur
1. **Démarrage** → Connexion automatique au serveur
2. **Identification** → Saisie nom/prénom/ID ± nouveau patient
3. **Login réussi** → Chargement des données (spécialités, médecins)
4. **Recherche** → Sélection critères + lancement
5. **Résultats** → Affichage consultations disponibles
6. **Réservation** → Sélection + saisie raison
7. **Confirmation** → Message succès + actualisation

## Gestion des erreurs
- Connexion serveur échouée
- Authentification refusée  
- Aucune consultation sélectionnée
- Réservation impossible (déjà prise)
- Perte de connexion réseau

## Compilation Qt
```bash
# Via Makefile
make

# Ou directement
g++ -fPIC -o ClientConsultationBooker *.cpp TCP/TCP.CPP `pkg-config --cflags --libs Qt5Widgets`
```

## Dépendances
- Qt5Widgets (interface graphique)
- Librairie TCP (../TCP/)
- Constantes (../util/const.h)