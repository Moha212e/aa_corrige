#ifndef TCP_H
#define TCP_H
#define TAILLE_MAX 1024
#include <sys/socket.h>
#include <netinet/in.h>

/**
 * Crée et configure un socket serveur pour écouter sur un port donné
 * 
 * @param port: Numéro de port sur lequel le serveur doit écouter
 * 
 * @return int: 
 *   - Descripteur de socket valide (> 0): Socket créé avec succès
 *   - -1: Erreur lors de la création du socket
 * 
 * Cette fonction sert à:
 * - Créer un socket TCP/IP
 * - Configurer l'adresse et le port d'écoute
 * - Mettre le socket en mode écoute (listen)
 * - Préparer le serveur à accepter des connexions
 * 
 * Note: Le socket doit être fermé avec close() après utilisation
 */
int ServerSocket(int port);

/**
 * Accepte une connexion client entrante sur un socket serveur
 * 
 * @param socketEcoute: Descripteur du socket serveur en écoute
 * @param ipClient: Buffer de sortie pour stocker l'adresse IP du client
 * 
 * @return int: 
 *   - Descripteur de socket client (> 0): Connexion acceptée avec succès
 *   - -1: Erreur lors de l'acceptation de la connexion
 * 
 * Cette fonction sert à:
 * - Accepter une nouvelle connexion client
 * - Récupérer l'adresse IP du client connecté
 * - Créer un socket dédié pour la communication avec ce client
 * - Permettre la communication bidirectionnelle
 * 
 * Note: Fonction bloquante - attend qu'un client se connecte
 */
int Accept(int socketEcoute, char* ipClient);

/**
 * Crée un socket client et se connecte à un serveur distant
 * 
 * @param ipServeur: Adresse IP du serveur à contacter
 * @param port: Numéro de port du serveur à contacter
 * 
 * @return int: 
 *   - Descripteur de socket valide (> 0): Connexion établie avec succès
 *   - -1: Erreur lors de la connexion au serveur
 * 
 * Cette fonction sert à:
 * - Créer un socket client TCP/IP
 * - Se connecter au serveur spécifié
 * - Établir une communication bidirectionnelle
 * - Permettre l'envoi et la réception de données
 * 
 * Note: Le socket doit être fermé avec close() après utilisation
 */
int ClientSocket(const char* ipServeur, int port);

/**
 * Envoie des données via un socket TCP
 * 
 * @param sSocket: Descripteur du socket pour l'envoi
 * @param data: Pointeur vers les données à envoyer
 * @param taille: Nombre d'octets à envoyer
 * 
 * @return int: 
 *   - Nombre d'octets envoyés (> 0): Envoi réussi
 *   - -1: Erreur lors de l'envoi
 * 
 * Cette fonction sert à:
 * - Transmettre des données via le réseau
 * - Gérer l'envoi complet des données
 * - Assurer la fiabilité de la transmission TCP
 * - Permettre la communication client-serveur
 * 
 * Note: Fonction bloquante jusqu'à l'envoi complet
 */
int Send(int sSocket, const char *data, int taille);

/**
 * Reçoit des données via un socket TCP
 * 
 * @param sSocket: Descripteur du socket pour la réception
 * @param data: Buffer de réception pour stocker les données
 * 
 * @return int: 
 *   - Nombre d'octets reçus (> 0): Réception réussie
 *   - 0: Connexion fermée par le client
 *   - -1: Erreur lors de la réception
 * 
 * Cette fonction sert à:
 * - Recevoir des données du réseau
 * - Stocker les données dans le buffer fourni
 * - Gérer la fin de connexion (retour 0)
 * - Permettre la communication bidirectionnelle
 * 
 * Note: Fonction bloquante jusqu'à la réception de données
 */
int Receive(int sSocket, char *data);

/**
 * Ferme proprement un socket TCP
 * 
 * @param sSocket: Descripteur du socket à fermer
 * 
 * @return int: 
 *   - 0: Fermeture réussie
 *   - -1: Erreur lors de la fermeture
 * 
 * Cette fonction sert à:
 * - Fermer proprement la connexion TCP
 * - Libérer les ressources système
 * - Terminer la communication
 * - Éviter les fuites de ressources
 * 
 * Note: Appelée automatiquement par close() dans la plupart des cas
 */
int closeSocket(int sSocket);

#endif