#ifndef TCP_H
#define TCP_H
#define TAILLE_MAX 1024
#include <sys/socket.h>
#include <netinet/in.h>

/**
 * Crée un socket serveur TCP et l'associe à un port
 * @param port Numéro de port sur lequel écouter
 * @return Descripteur du socket serveur, -1 en cas d'erreur
 */
int ServerSocket(int port);

/**
 * Accepte une connexion entrante sur un socket serveur
 * @param socketEcoute Socket serveur en écoute
 * @param ipClient Buffer pour stocker l'adresse IP du client (optionnel, peut être NULL)
 * @return Descripteur du socket client, -1 en cas d'erreur
 */
int Accept(int socketEcoute, char* ipClient);

/**
 * Crée un socket client et se connecte à un serveur
 * @param ipServeur Adresse IP du serveur à contacter
 * @param port Numéro de port du serveur
 * @return Descripteur du socket client connecté, -1 en cas d'erreur
 */
int ClientSocket(const char* ipServeur, int port);

/**
 * Envoie des données via un socket TCP
 * @param sSocket Descripteur du socket
 * @param data Données à envoyer
 * @param taille Nombre d'octets à envoyer
 * @return Nombre d'octets envoyés, -1 en cas d'erreur
 */
int Send(int sSocket, const char *data, int taille);

/**
 * Reçoit des données via un socket TCP (jusqu'au caractère '\n')
 * @param sSocket Descripteur du socket
 * @param data Buffer pour stocker les données reçues
 * @return Nombre d'octets reçus, -1 en cas d'erreur
 */
int Receive(int sSocket, char *data);

/**
 * Ferme un socket TCP
 * @param sSocket Descripteur du socket à fermer
 * @return 0 en cas de succès, -1 en cas d'erreur
 */
int closeSocket(int sSocket);

#endif