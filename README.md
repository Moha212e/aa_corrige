# 🏥 Système de Réservation de Consultations Hospitalières

## 📋 Description du Projet

Système complet de réservation de consultations médicales composé d'une application client Qt, d'un serveur multi-threadé en C/C++, et d'une base de données MySQL. Le projet implémente une architecture client-serveur robuste pour la gestion des réservations dans un environnement hospitalier.

## 🎯 Objectifs

- **Patients** : Interface conviviale pour réserver des consultations
- **Hôpital** : Gestion centralisée des réservations
- **Médecins** : Planification automatisée des consultations
- **Administration** : Monitoring des connexions et activités

## 🏗️ Architecture Générale

```
┌─────────────────┐    TCP/CBP    ┌─────────────────┐    MySQL    ┌─────────────────┐
│   Client Qt     │ ◄──────────── │ Serveur C/C++   │ ◄────────── │  Base de        │
│  (Bornes)       │               │ Multi-threadé   │             │  Données        │
└─────────────────┘               └─────────────────┘             └─────────────────┘
      ▲                                    ▲                            ▲
      │                                    │                            │
┌─────▼─────┐                        ┌────▼────┐                  ┌─────▼─────┐
│  TCP Lib  │                        │   CBP   │                  │ CreationBD│
│ (Sockets) │                        │Protocol │                  │ (Setup)   │
└───────────┘                        └─────────┘                  └───────────┘
```

## 📁 Structure du Projet

### 🔌 [TCP/](./TCP/) - Librairie de Communication
- **Rôle** : Abstraction des sockets TCP/IP
- **Caractéristiques** : Générique, réutilisable, thread-safe
- **Fonctions** : ServerSocket, ClientSocket, Send, Receive

### 🖥️ [serveur/](./serveur/) - Serveur de Réservation
- **Rôle** : Traitement des demandes de réservation
- **Architecture** : Pool de threads POSIX
- **Configuration** : `serveur.conf` (port, threads, BD)
- **Protocole** : CBP (Consultation Booking Protocol)

### 📡 [protocole/](./protocole/) - Implémentation CBP
- **Rôle** : Logique métier et communication serveur-client
- **Commandes** : LOGIN, LOGOUT, SEARCH, BOOK, GET_*
- **Sécurité** : Authentification, sessions, thread-safety
- **Base de données** : Gestion MySQL intégrée

### 💻 [ClientConsultationBookerQt/](./ClientConsultationBookerQt/) - Interface Utilisateur
- **Rôle** : Borne d'accueil patient interactive
- **Technologie** : Qt5 Widgets
- **Fonctionnalités** : Identification, recherche, réservation
- **UX** : Interface intuitive et responsive

### 🗄️ [BD_Hospital/](./BD_Hospital/) - Base de Données
- **Rôle** : Initialisation et données de test
- **Tables** : patients, doctors, specialties, consultations
- **Programme** : CreationBD automatisé
- **Données** : Jeu de test complet

### ⚙️ [util/](./util/) - Utilitaires Partagés
- **Rôle** : Constantes et définitions communes
- **Contenu** : Protocole CBP, codes erreur, configuration
- **Usage** : Partagé par tous les composants

## 🚀 Installation et Compilation

### Prérequis
```bash
# Système
sudo apt-get update
sudo apt-get install build-essential

# Qt5 pour l'interface
sudo apt-get install qt5-default qtbase5-dev

# MySQL pour la base de données
sudo apt-get install mysql-server mysql-client
sudo apt-get install libmysqlclient-dev

# Threads POSIX (généralement inclus)
sudo apt-get install libc6-dev
```

### Compilation
```bash
# Compilation complète
make

# Ou composants individuels
make serveur      # Serveur uniquement
make client       # Client Qt uniquement  
make bd           # Programme BD uniquement
```

### Configuration MySQL
```sql
-- Créer la base et utilisateur
CREATE DATABASE PourStudent;
CREATE USER 'Student'@'localhost' IDENTIFIED BY 'PassStudent1_';
GRANT ALL PRIVILEGES ON PourStudent.* TO 'Student'@'localhost';
FLUSH PRIVILEGES;
```

## 🎮 Utilisation

### 1. Initialisation de la Base de Données
```bash
# Créer et peupler la BD
./BD_Hospital/CreationBD
```

### 2. Démarrage du Serveur
```bash
# Avec configuration par défaut
./serveur/serveur

# Avec configuration personnalisée
./serveur/serveur ma_config.conf
```

### 3. Lancement du Client
```bash
# Interface graphique Qt
./ClientConsultationBookerQt/ClientConsultationBooker
```

## 📋 Fonctionnalités Principales

### Pour les Patients
- ✅ **Identification** : Nouveau patient ou existant
- ✅ **Recherche** : Par spécialité, médecin, dates
- ✅ **Réservation** : Sélection et raison de consultation
- ✅ **Interface intuitive** : Qt moderne et accessible

### Pour le Système
- ✅ **Multi-threading** : Gestion simultanée de multiples clients
- ✅ **Thread-safety** : Accès concurrent sécurisé aux ressources
- ✅ **Configuration** : Paramètres externalisés
- ✅ **Monitoring** : Logs détaillés et traçabilité

### Protocole CBP
- ✅ **LOGIN** : Authentification patient
- ✅ **GET_SPECIALTIES** : Liste des spécialités
- ✅ **GET_DOCTORS** : Liste des médecins
- ✅ **SEARCH_CONSULTATIONS** : Recherche avec critères
- ✅ **BOOK_CONSULTATION** : Réservation avec raison
- ✅ **LOGOUT** : Déconnexion propre

## 📊 Données de Test

### Spécialités Disponibles
- Cardiologie
- Dermatologie  
- Généraliste
- Pédiatrie
- Psychiatrie

### Médecins Inclus
- Dr. Dubois Pierre (Cardiologie)
- Dr. Martin Claire (Dermatologie)
- Dr. Durand Michel (Généraliste)
- Dr. Leroy Anne (Pédiatrie)
- Dr. Moreau Jean (Psychiatrie)
- Dr. Simon Marie (Généraliste)

### Consultations
- **Période** : Octobre 2025
- **Créneaux** : 9h00 à 14h30
- **État** : Mix de disponibles/réservées

## 🔧 Configuration

### Serveur (`serveur/serveur.conf`)
```properties
PORT_RESERVATION=8080         # Port d'écoute
NB_THREADS_POOL=4            # Threads dans le pool
TAILLE_FILE_ATTENTE=20       # File d'attente
DB_HOST=localhost            # Serveur MySQL
DB_USER=Student              # Utilisateur BD
DB_PASSWORD=PassStudent1_    # Mot de passe
DB_NAME=PourStudent          # Base de données
```

### Client (constants dans `util/const.h`)
```c
#define DEFAULT_SERVER_IP "127.0.0.1"
#define DEFAULT_SERVER_PORT 8080
```

## 🐛 Dépannage

### Problèmes de Compilation
```bash
# Vérifier les dépendances Qt
pkg-config --cflags --libs Qt5Widgets

# Vérifier MySQL
mysql-config --cflags --libs
```

### Problèmes de Connexion
```bash
# Tester MySQL
mysql -u Student -p -h localhost PourStudent

# Vérifier le serveur
netstat -tulpn | grep :8080
```

### Problèmes d'Exécution
```bash
# Logs du serveur
./serveur/serveur 2>&1 | tee serveur.log

# Debug client Qt
QT_LOGGING_RULES="*.debug=true" ./ClientConsultationBookerQt/ClientConsultationBooker
```

## 🔮 Extensions Futures

### Prévu dans les Consignes
- **Client Java Admin** : Monitoring des connexions
- **Port dédié** : Pour administration
- **Rapports avancés** : Statistiques d'utilisation

### Améliorations Possibles
- **Sécurité** : Chiffrement des communications
- **Scalabilité** : Load balancing, clustering
- **Mobile** : Application smartphone
- **Web** : Interface navigateur

## 👥 Équipe de Développement
- **Développeur** : Moha212e
- **Email** : mohammed.elaouali@student.hepl.be
- **Institution** : HEPL - DACSC 2025-2026

## 📄 Licence
Projet académique - HEPL 2025

---

## 🎯 Conformité Cahier des Charges

✅ **Application Client Qt** : Interface complète et fonctionnelle  
✅ **Serveur Multi-threadé** : Pool de threads POSIX  
✅ **Protocole CBP** : Toutes les commandes implémentées  
✅ **Base de données MySQL** : Structure et données conformes  
✅ **Librairie TCP** : Générique et abstraite  
✅ **Configuration externalisée** : Fichier serveur.conf  
✅ **Thread-safety** : Mutex et synchronisation  
✅ **Gestion d'erreurs** : Codes de retour appropriés  

**Status** : ✅ **PROJET COMPLET ET CONFORME**