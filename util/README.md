# Utilitaires - Constantes partagées

## Description
Fichier de constantes et définitions partagées par tous les composants du projet.

## Fichiers
- **`const.h`** : Toutes les constantes du projet

## Contenu

### Commandes du protocole CBP
```c
#define LOGIN "LOGIN"
#define LOGOUT "LOGOUT"
#define GET_SPECIALTIES "GET_SPECIALTIES"
#define GET_DOCTORS "GET_DOCTORS" 
#define SEARCH_CONSULTATIONS "SEARCH_CONSULTATIONS"
#define BOOK_CONSULTATION "BOOK_CONSULTATION"
```

### Réponses protocole
```c
#define OK "ok"
#define KO "ko"
```

### Constantes interface
```c
#define TOUS "--- TOUS ---"
#define TOUTES "--- TOUTES ---"
#define diez "#"
#define pipeSeparator "|"
```

### Codes d'erreur
```c
#define SUCCES 0
#define MAUVAIS_IDENTIFIANTS 1
#define PATIENT_NON_TROUVE 2
#define ERREUR_BD 3
#define ERREUR_INCONNUE 4
#define FERMER_CONNEXION 99
```

### Tailles des buffers
```c
#define SMALL_BUF 100
#define MED_BUF 200
#define BIG_BUF 1024
#define HUGE_BUF 2048
```

### Configuration réseau
```c
#define DEFAULT_SERVER_IP "127.0.0.1"
#define DEFAULT_SERVER_PORT 8080
```

### Configuration base de données
```c
#define DB_HOST "localhost"
#define DB_USER "Student"
#define DB_PASS "PassStudent1_"
#define DB_NAME "PourStudent"
```

## Utilisation
```c
#include "../util/const.h"

// Utilisation des constantes
sprintf(requete, "%s%s%s%s%s", LOGIN, diez, nom, diez, prenom);
if (resultat == SUCCES) {
    // Traitement...
}
```