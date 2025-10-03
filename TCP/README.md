# TCP - Librairie de Sockets

## Description
Librairie générique et abstraite pour la communication TCP/IP en C/C++.

## Objectif
Fournir une interface simple et réutilisable pour les communications réseau, en masquant la complexité des structures système (`sockaddr_in`, etc.).

## Fichiers
- **`TCP.h`** : Déclarations des fonctions de la librairie
- **`TCP.CPP`** : Implémentation des fonctions réseau

## Fonctions principales

### Côté Serveur
- `ServerSocket(int port)` : Crée un socket serveur sur un port donné
- `Accept(int socketEcoute, char* ipClient)` : Accepte une connexion client

### Côté Client  
- `ClientSocket(const char* ipServeur, int port)` : Se connecte à un serveur

### Communication
- `Send(int sSocket, const char* data, int taille)` : Envoie des données
- `Receive(int sSocket, char* data)` : Reçoit des données
- `closeSocket(int sSocket)` : Ferme une connexion

## Caractéristiques
- ✅ **Générique** : Aucune notion métier (patients, consultations)
- ✅ **Abstraite** : Cache les détails système
- ✅ **Réutilisable** : Peut être utilisée dans d'autres projets
- ✅ **Protocole de fin** : Utilise `\n` comme marqueur de fin de message

## Utilisation
```cpp
#include "TCP.h"

// Serveur
int serveur = ServerSocket(8080);
int client = Accept(serveur, ipClient);

// Client  
int socket = ClientSocket("127.0.0.1", 8080);

// Communication
Send(socket, "Hello", 5);
int recu = Receive(socket, buffer);
```