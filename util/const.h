/**
 * @file const.h
 * @brief Définitions des constantes utilisées dans le système de réservation de consultations
 * 
 * Ce fichier contient toutes les constantes nécessaires au fonctionnement du protocole CBP :
 * - Commandes du protocole de communication
 * - Codes de retour et messages de statut
 * - Paramètres de configuration (base de données, réseau, interface)
 * - Tailles de buffers et formats de données
 */

// ============================================================================
// CONSTANTES DU PROTOCOLE CBP (Consultation Booking Protocol)
// ============================================================================

// Constantes pour les requêtes d'authentification
#define LOGIN "LOGIN"                    // Commande de connexion/inscription patient
#define LOGOUT "LOGOUT"                  // Commande de déconnexion

// Constantes pour les requêtes de récupération de données
#define GET_SPECIALTIES "GET_SPECIALTIES"      // Récupère la liste des spécialités médicales
#define GET_DOCTORS "GET_DOCTORS"              // Récupère la liste des médecins
#define GET_PATIENTS "GET_PATIENTS"            // Récupère la liste des patients
#define GET_CONSULTATIONS "GET_CONSULTATIONS"  // Récupère la liste des consultations
#define GET_CONSULTATION "GET_CONSULTATION"    // Récupère les détails d'une consultation

// Constantes pour les requêtes de recherche et réservation
#define SEARCH_CONSULTATIONS "SEARCH_CONSULTATIONS"  // Recherche de consultations disponibles
#define BOOK_CONSULTATION "BOOK_CONSULTATION"        // Réservation d'une consultation

// ============================================================================
// CODES DE STATUT ET RÉPONSES
// ============================================================================

#define KO "ko"                          // Code de retour : échec
#define OK "ok"                          // Code de retour : succès

// Valeurs par défaut pour les filtres de recherche
#define TOUS "--- TOUS ---"              // Option "tous" pour les médecins
#define TOUTES "--- TOUTES ---"          // Option "toutes" pour les spécialités

// Séparateurs de protocole
#define diez "#"                         // Séparateur principal des champs
#define pipeSeparator "|"               // Séparateur pour les listes d'éléments

// ============================================================================
// CONFIGURATION DATES ET FORMATS
// ============================================================================

#define DEFAULT_DATE_FORMAT "yyyy-MM-dd"     // Format standard pour les dates
#define DEFAULT_DATE_DEBUT "2025-09-15"      // Date de début par défaut pour les recherches
#define DEFAULT_DATE_FIN "2025-12-31"        // Date de fin par défaut pour les recherches

// Flags d'authentification
#define NOUVEAU_PATIENT "1"                  // Indicateur : nouveau patient (inscription)
#define PATIENT_EXISTANT "0"                 // Indicateur : patient existant (connexion)

// ============================================================================
// CODES D'ERREUR ET STATUTS
// ============================================================================

#define SUCCES 0                         // Opération réussie
#define MAUVAIS_IDENTIFIANTS 1           // Identifiants incorrects
#define PATIENT_NON_TROUVE 2             // Patient introuvable en base
#define ERREUR_BD 3                      // Erreur de base de données
#define ERREUR_INCONNUE 4                // Erreur non spécifiée
#define FERMER_CONNEXION 99              // Code pour fermer la connexion client
// ============================================================================
// TAILLES DE DONNÉES ET BUFFERS
// ============================================================================

// Longueurs maximales des champs de données
#define MAX_NAME_LEN 50                  // Longueur max nom/prénom
#define MAX_ID_LEN 10                    // Longueur max identifiants numériques
#define FLAG_LEN 2                       // Longueur des flags (0/1)

// Tailles des buffers de communication
#define SMALL_BUF 100                    // Petit buffer (messages courts)
#define MED_BUF 200                      // Buffer moyen (une ligne de données)
#define BIG_BUF 1024                     // Gros buffer (requêtes SQL, réponses)
#define HUGE_BUF 2048                    // Très gros buffer (listes importantes)

#define IP_STR_LEN 50                    // Longueur max adresse IP (IPv4/IPv6)

// ============================================================================
// CONFIGURATION BASE DE DONNÉES
// ============================================================================

#define DB_HOST "localhost"              // Serveur de base de données
#define DB_USER "Student"                // Nom d'utilisateur MySQL
#define DB_PASS "PassStudent1_"          // Mot de passe MySQL
#define DB_NAME "PourStudent"            // Nom de la base de données

// ============================================================================
// CONFIGURATION RÉSEAU
// ============================================================================

#define DEFAULT_SERVER_IP "127.1.1.1"    // Adresse IP du serveur par défaut
#define DEFAULT_SERVER_PORT 8080         // Port d'écoute du serveur

// ============================================================================
// CONFIGURATION INTERFACE UTILISATEUR (Qt)
// ============================================================================

// Paramètres du tableau des consultations
#define COL_COUNT_CONSULTATIONS 5        // Nombre de colonnes dans le tableau
#define COL_WIDTH_ID 40                  // Largeur colonne ID consultation
#define COL_WIDTH_SPECIALTY 150          // Largeur colonne spécialité
#define COL_WIDTH_DOCTOR 200             // Largeur colonne médecin
#define COL_WIDTH_DATE 150               // Largeur colonne date
#define COL_WIDTH_HOUR 100               // Largeur colonne heure

// Longueurs des chaînes de date/heure
#define DATE_STR_LEN 20                  // Longueur max date (yyyy-MM-dd)
#define HOUR_STR_LEN 16                  // Longueur max heure (HH:mm:ss)