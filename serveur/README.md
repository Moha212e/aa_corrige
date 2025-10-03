# Serveur de Réservation

## Description
Serveur multi-threadé en C gérant les demandes de réservation de consultations médicales via le protocole CBP.

## Architecture
- **Type** : Serveur de connexions (pas de requêtes isolées)
- **Multi-threading** : Pool de threads POSIX 
- **Modèle** : Pool de threads avec file d'attente
- **Configuration** : Via fichier `serveur.conf`

## Fichiers
- **`serveur.cpp`** : Code principal du serveur
- **`serveur.conf`** : Configuration (port, nombre de threads, etc.)

## Configuration (`serveur.conf`)
```properties
PORT_RESERVATION=8080         # Port d'écoute
NB_THREADS_POOL=4            # Nombre de threads dans le pool
TAILLE_FILE_ATTENTE=20       # Taille file d'attente connexions
DB_HOST=localhost            # Serveur MySQL
DB_USER=Student              # Utilisateur BD
DB_PASSWORD=PassStudent1_    # Mot de passe BD
DB_NAME=PourStudent          # Nom de la base
```

## Fonctionnalités
- ✅ **Pool de threads** configurables
- ✅ **Gestion des signaux** (arrêt propre avec SIGINT)
- ✅ **File d'attente** des connexions 
- ✅ **Configuration externalisée**
- ✅ **Logs détaillés** avec ID des threads

## Architecture technique
1. **Thread principal** : Accepte les connexions
2. **Pool de workers** : Traite les requêtes CBP
3. **File partagée** : Synchronisation thread-safe
4. **Mutex/Conditions** : Coordination entre threads

## Utilisation
```bash
# Compilation
make

# Démarrage avec config par défaut
./serveur/serveur

# Démarrage avec config custom
./serveur/serveur mon_serveur.conf
```

## Dépendances
- Librairie TCP (../TCP/)
- Protocole CBP (../protocole/)
- MySQL C API
- POSIX Threads