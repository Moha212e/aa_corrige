#ifndef CBP_H
#define CBP_H

#define NB_MAX_CLIENTS 100

/**
 * Fonction principale du protocole CBP (Consultation Booking Protocol)
 * 
 * @param requete: Chaîne de caractères contenant la requête du client (format: "COMMANDE#param1#param2#...")
 * @param reponse: Buffer de sortie où sera stockée la réponse formatée
 * @param socket: Descripteur de socket du client pour identifier la session
 * 
 * @return bool: 
 *   - true: La connexion doit continuer (requête traitée avec succès)
 *   - false: La connexion doit se fermer (LOGOUT ou erreur)
 * 
 * Cette fonction sert de routeur principal qui:
 * - Parse la requête pour identifier la commande
 * - Vérifie l'authentification du client
 * - Exécute la commande appropriée
 * - Formate la réponse selon le protocole CBP
 * - Gère les erreurs et les cas d'exception
 */
int CBP(char* requete, char* reponse, int socket);

/**
 * Fonction d'authentification et gestion des patients
 * 
 * @param nom: Nom de famille du patient
 * @param prenom: Prénom du patient
 * @param numeroPatient: Numéro de patient (si patient existant), -1 si nouveau
 * @param nouveauPatient: 1 si nouveau patient, 0 si patient existant
 * @param patientId: Pointeur pour retourner l'ID du patient (en sortie)
 * 
 * @return int: 
 *   - SUCCES: Authentification réussie
 *   - MAUVAIS_IDENTIFIANTS: Identifiants invalides
 *   - PATIENT_NON_TROUVE: Patient existant non trouvé
 *   - ERREUR_BD: Erreur base de données
 *   - ERREUR_INCONNUE: Erreur générale
 * 
 * Cette fonction sert à:
 * - Vérifier l'existence d'un patient existant (nom/prénom/ID)
 * - Créer un nouveau patient en BD et retourner son ID
 * - Valider les informations d'authentification
 * - Gérer les cas d'erreur d'accès BD
 */
int CBP_Login(const char* nom, const char* prenom, int numeroPatient, int nouveauPatient, int* patientId);

/**
 * Fonction de calcul mathématique (fonction utilitaire)
 * 
 * @param op: Opérateur mathématique ('+', '-', '*', '/')
 * @param a: Premier opérande
 * @param b: Deuxième opérande
 * 
 * @return int: Résultat de l'opération mathématique
 * 
 * Cette fonction sert à:
 * - Effectuer des calculs mathématiques basiques
 * - Gérer les opérations arithmétiques
 * - Lancer une exception en cas de division par zéro
 * 
 * Note: Fonction utilitaire, pas directement liée au protocole de réservation
 */
int CBP_Operation(char op, int a, int b);

/**
 * Récupère la liste des patients connectés (pour client admin Java)
 * 
 * @param buffer: Buffer de sortie pour stocker la liste formatée
 * @param tailleBuff: Taille du buffer de sortie
 * 
 * @return int: Nombre de patients connectés
 * 
 * Cette fonction sert à:
 * - Fournir la liste des patients connectés au client admin
 * - Formater les informations (ID, nom, prénom, IP) 
 * - Permettre le monitoring des connexions actives
 * - Sécuriser l'accès aux informations des patients
 * 
 * Note: Format de sortie: "patientId#nom#prenom#ip|..."
 */
int CBP_GetPatientsConnectes(char* buffer, int tailleBuff);

/**
 * Ferme proprement toutes les connexions et libère les ressources
 * 
 * @return void: Aucune valeur de retour
 * 
 * Cette fonction sert à:
 * - Fermer toutes les connexions client actives
 * - Libérer les ressources du serveur
 * - Déconnecter proprement de la base de données
 * - Assurer un arrêt propre du serveur
 * 
 * Note: Appelée lors de l'arrêt du serveur ou en cas d'erreur critique
 */
void CBP_Close();

/**
 * Établit une connexion à la base de données MySQL
 * 
 * @return int: 
 *   - 0: Connexion réussie
 *   - -1: Erreur de connexion
 * 
 * Cette fonction sert à:
 * - Initialiser la connexion MySQL
 * - Se connecter à la base de données "PourStudent"
 * - Configurer les paramètres de connexion (localhost, Student, PassStudent1_)
 * - Gérer les erreurs de connexion
 * 
 * Note: Utilise un mutex pour la sécurité des threads
 */
int connecterBD();

/**
 * Ferme la connexion à la base de données MySQL
 * 
 * @return void: Aucune valeur de retour
 * 
 * Cette fonction sert à:
 * - Fermer proprement la connexion MySQL
 * - Libérer les ressources de la base de données
 * - Assurer un arrêt propre du serveur
 * - Éviter les fuites de mémoire
 * 
 * Note: Appelée lors de l'arrêt du serveur ou en cas d'erreur
 */
void deconnecterBD();

#endif 