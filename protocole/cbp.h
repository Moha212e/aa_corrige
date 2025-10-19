#ifndef CBP_H
#define CBP_H

#define NB_MAX_CLIENTS 100

/**
 * Fonction principale du protocole CBP (Consultation Booking Protocol)
 * Traite les requêtes clients et génère les réponses appropriées
 * @param requete La requête reçue du client (format: COMMANDE#param1#param2#...)
 * @param reponse Buffer pour stocker la réponse à envoyer au client
 * @param socket Socket de communication avec le client
 * @return true si la connexion doit rester ouverte, false si elle doit être fermée
 */
bool CBP(char* requete, char* reponse, int socket);

/**
 * Fonction principale du protocole ACBP (Admin Consultation Booking Protocol)
 * Traite les requêtes admin et génère les réponses appropriées
 * @param requete La requête reçue du client admin (format: COMMANDE#param1#param2#...)
 * @param reponse Buffer pour stocker la réponse à envoyer au client admin
 * @param socket Socket de communication avec le client admin
 * @return true si la connexion doit rester ouverte, false si elle doit être fermée
 */
bool ACBP(char* requete, char* reponse, int socket);

/**
 * Gère l'authentification/inscription d'un patient
 * @param nom Nom de famille du patient
 * @param prenom Prénom du patient
 * @param numeroPatient ID du patient (pour connexion existante)
 * @param nouveauPatient 1 si nouveau patient, 0 si patient existant
 * @param patientId Pointeur vers l'ID du patient (sortie)
 * @return Code de retour (SUCCES, PATIENT_NON_TROUVE, ERREUR_BD)
 */
int CBP_Login(const char* nom, const char* prenom, int numeroPatient, int nouveauPatient, int* patientId);

/**
 * Ferme proprement le protocole CBP
 * Ferme toutes les connexions clients et déconnecte la base de données
 */
void CBP_Close();

/**
 * Établit la connexion à la base de données MySQL
 * @return 0 en cas de succès, -1 en cas d'erreur
 */
int connecterBD();

/**
 * Ferme la connexion à la base de données MySQL
 */
void deconnecterBD();

#endif 