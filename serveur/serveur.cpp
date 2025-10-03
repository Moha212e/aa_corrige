#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include "../TCP/TCP.h"
#include "../protocole/cbp.h"
#include "../util/const.h"

/**
 * Gestionnaire de signal pour l'arrêt propre du serveur
 * 
 * @param s: Numéro du signal reçu (SIGINT)
 * 
 * @return void: Aucune valeur de retour
 * 
 * Cette fonction sert à:
 * - Intercepter le signal SIGINT (Ctrl+C)
 * - Fermer proprement toutes les connexions
 * - Libérer les ressources du serveur
 * - Assurer un arrêt propre du système
 * 
 * Note: Appelée automatiquement lors de l'interruption du serveur
 */
void HandlerSIGINT(int s);

/**
 * Traite une connexion client individuelle
 * 
 * @param sService: Descripteur de socket du client à traiter
 * 
 * @return void: Aucune valeur de retour
 * 
 * Cette fonction sert à:
 * - Recevoir les requêtes du client
 * - Traiter les requêtes via le protocole CBP
 * - Envoyer les réponses au client
 * - Gérer la fin de connexion
 * 
 * Note: Fonction appelée par les threads du pool
 */
void TraitementConnexion(int sService);

/**
 * Fonction principale des threads du pool de traitement
 * 
 * @param p: Pointeur vers les paramètres du thread (non utilisé)
 * 
 * @return void*: Pointeur de retour (toujours NULL)
 * 
 * Cette fonction sert à:
 * - Attendre les connexions dans la file d'attente
 * - Traiter les connexions client une par une
 * - Gérer le cycle de vie des connexions
 * - Maintenir l'activité du thread
 * 
 * Note: Fonction exécutée en boucle infinie par chaque thread
 */
void* FctThreadClient(void* p);

/**
 * Charge la configuration du serveur depuis un fichier
 * 
 * @param nomFichier: Nom du fichier de configuration à charger
 * 
 * @return int: 
 *   - 0: Chargement réussi
 *   - -1: Erreur lors du chargement
 * 
 * Cette fonction sert à:
 * - Lire le fichier de configuration
 * - Parser les paramètres (port, threads, file d'attente)
 * - Initialiser les variables globales
 * - Valider la configuration
 * 
 * Note: Format du fichier: "cle=valeur" (une par ligne)
 */
int chargerConfiguration(const char* nomFichier);

// Variables de configuration (seront chargées depuis le fichier .conf)
int PORT_RESERVATION;
int NB_THREADS_POOL;
int TAILLE_FILE_ATTENTE;

int sEcoute;

int* socketsAcceptees;
int indiceEcriture = 0, indiceLecture = 0;
pthread_mutex_t mutexSocketsAcceptees;
pthread_cond_t condSocketsAcceptees; 
/**
 * Fonction principale du serveur de réservation de consultations
 * 
 * @param argc: Nombre d'arguments de la ligne de commande
 * @param argv: Tableau des arguments (argv[0] = nom du programme, argv[1] = fichier config optionnel)
 * 
 * @return int: 
 *   - 0: Arrêt normal du serveur
 *   - 1: Erreur lors de l'initialisation
 * 
 * Cette fonction sert à:
 * - Charger la configuration depuis un fichier
 * - Initialiser le socket d'écoute
 * - Créer le pool de threads de traitement
 * - Gérer les connexions client en boucle infinie
 * - Assurer l'arrêt propre du serveur
 * 
 * Note: Le serveur fonctionne en mode multi-thread avec une file d'attente
 */
int main(int argc, char* argv[])
{
    // Chargement obligatoire du fichier de configuration
    const char* fichierConfig = (argc == 2) ? argv[1] : "serveur.conf";
    
    printf("Chargement du fichier de configuration : %s\n", fichierConfig);
    if (chargerConfiguration(fichierConfig) != 0)
    {
        printf("Erreur lors du chargement de la configuration\n");
        printf("Usage: %s [fichier_config]\n", argv[0]);
        exit(1);
    }
    
    printf("Configuration chargée depuis le fichier :\n");
    printf("  Port: %d\n", PORT_RESERVATION);
    printf("  Threads: %d\n", NB_THREADS_POOL);
    printf("  File d'attente: %d\n", TAILLE_FILE_ATTENTE);
    
    // Allocation dynamique du tableau de sockets
    socketsAcceptees = (int*)malloc(TAILLE_FILE_ATTENTE * sizeof(int));
    if (socketsAcceptees == NULL)
    {
        printf("Erreur d'allocation mémoire\n");
        exit(1);
    }

    
    
    // Initialisation socketsAcceptees
    pthread_mutex_init(&mutexSocketsAcceptees, NULL);
    pthread_cond_init(&condSocketsAcceptees, NULL);
    for (int i = 0; i < TAILLE_FILE_ATTENTE; i++)
        socketsAcceptees[i] = -1;
    
    // Armement des signaux
    struct sigaction A;
    A.sa_flags = 0;
    sigemptyset(&A.sa_mask);
    A.sa_handler = HandlerSIGINT;
    if (sigaction(SIGINT, &A, NULL) == -1)
    {
        perror("Erreur de sigaction");
        exit(1);
    }
    
    // Creation de la socket d'écoute
    printf("Tentative de création du socket sur le port %d...\n", PORT_RESERVATION);
    if ((sEcoute = ServerSocket(PORT_RESERVATION)) == -1)
    {
        perror("Erreur de ServeurSocket");
        printf("Impossible de créer le socket sur le port %d\n", PORT_RESERVATION);
        exit(1);
    }
    printf("Socket créé avec succès sur le port %d\n", PORT_RESERVATION);
    
    // Creation du pool de threads
    printf("Création du pool de threads.\n");
    pthread_t th;
    for (int i = 0; i < NB_THREADS_POOL; i++)
        pthread_create(&th, NULL, FctThreadClient, NULL);
    
    // Mise en boucle du serveur
    int sService;
    char ipClient[50];
    printf("Demarrage du serveur.\n");
    while (1)
    {
        printf("Attente d'une connexion...\n");
        if ((sService = Accept(sEcoute, ipClient)) == -1)
        {
            perror("Erreur de Accept");
            close(sEcoute);
            CBP_Close();
            exit(1);
        }
        printf("Connexion acceptée : IP=%s socket=%d\n", ipClient, sService);
        
        // Insertion en liste d'attente et réveil d'un thread du pool
        // (Production d'une tâche)
        pthread_mutex_lock(&mutexSocketsAcceptees);
        socketsAcceptees[indiceEcriture] = sService; // !!!
        indiceEcriture++;
        if (indiceEcriture == TAILLE_FILE_ATTENTE) indiceEcriture = 0;
        pthread_mutex_unlock(&mutexSocketsAcceptees);
        pthread_cond_signal(&condSocketsAcceptees);
    }
} 

/**
 * Fonction principale des threads du pool de traitement
 * 
 * @param p: Pointeur vers les paramètres du thread (non utilisé)
 * 
 * @return void*: Pointeur de retour (toujours NULL)
 * 
 * Cette fonction sert à:
 * - Attendre les connexions dans la file d'attente
 * - Traiter les connexions client une par une
 * - Gérer le cycle de vie des connexions
 * - Maintenir l'activité du thread
 * 
 * Note: Fonction exécutée en boucle infinie par chaque thread
 */
void* FctThreadClient(void* p)
{
    (void)p; // Supprimer l'avertissement du paramètre inutilisé
    int sService;
    while (1)
    {
        printf("\t[THREAD %lu] Attente socket...\n", (unsigned long)pthread_self());
        
        // Attente d'une tâche
        pthread_mutex_lock(&mutexSocketsAcceptees);
        while (indiceEcriture == indiceLecture)
            pthread_cond_wait(&condSocketsAcceptees, &mutexSocketsAcceptees);
        sService = socketsAcceptees[indiceLecture];
        socketsAcceptees[indiceLecture] = -1;
        indiceLecture++;
        if (indiceLecture == TAILLE_FILE_ATTENTE) indiceLecture = 0;
        pthread_mutex_unlock(&mutexSocketsAcceptees);
        
        // Traitement de la connexion (consommation de la tâche)
        printf("\t[THREAD %lu] Je m'occupe de la socket %d\n",
               (unsigned long)pthread_self(), sService);
        TraitementConnexion(sService);
    }
} 

/**
 * Gestionnaire de signal pour l'arrêt propre du serveur
 * 
 * @param s: Numéro du signal reçu (SIGINT)
 * 
 * @return void: Aucune valeur de retour
 * 
 * Cette fonction sert à:
 * - Intercepter le signal SIGINT (Ctrl+C)
 * - Fermer proprement toutes les connexions
 * - Libérer les ressources du serveur
 * - Assurer un arrêt propre du système
 * 
 * Note: Appelée automatiquement lors de l'interruption du serveur
 */
void HandlerSIGINT(int s)
{
    (void)s; // Supprimer l'avertissement du paramètre inutilisé
    printf("\nArret du serveur.\n");
    close(sEcoute);
    pthread_mutex_lock(&mutexSocketsAcceptees);
    for (int i = 0; i < TAILLE_FILE_ATTENTE; i++)
        if (socketsAcceptees[i] != -1) close(socketsAcceptees[i]);
    pthread_mutex_unlock(&mutexSocketsAcceptees);
    CBP_Close();
    exit(0);
} 

/**
 * Traite une connexion client individuelle
 * 
 * @param sService: Descripteur de socket du client à traiter
 * 
 * @return void: Aucune valeur de retour
 * 
 * Cette fonction sert à:
 * - Recevoir les requêtes du client
 * - Traiter les requêtes via le protocole CBP
 * - Envoyer les réponses au client
 * - Gérer la fin de connexion
 * 
 * Note: Fonction appelée par les threads du pool
 */
void TraitementConnexion(int sService)
{
        char requete[200], reponse[200];
        int nbLus, nbEcrits;
        int status = SUCCES;

        while (1)
        {
            printf("\t[THREAD %lu] Attente requete...\n", (unsigned long)pthread_self());

            // ***** Reception Requete ******************
            if ((nbLus = Receive(sService, requete)) < 0)
            {
                perror("Erreur de Receive");
                close(sService);
                return; // Seulement fermer cette connexion, pas tout le serveur
            }

            // ***** Fin de connexion ? *****************
            if (nbLus == 0)
            {
                printf("\t[THREAD %lu] Fin de connexion du client.\n", (unsigned long)pthread_self());
                close(sService);
                return;
            }

            requete[nbLus] = 0;
            printf("\t[THREAD %lu] Requete recue = %s\n", (unsigned long)pthread_self(), requete);

            // ***** Traitement de la requete ***********
            status = CBP(requete, reponse, sService);

            // ***** Envoi de la reponse ****************
            if ((nbEcrits = Send(sService, reponse, strlen(reponse))) < 0)
            {
                perror("Erreur de Send");
                close(sService);
                return; // Seulement fermer cette connexion, pas tout le serveur
            }

            printf("\t[THREAD %lu] Reponse envoyee = %s\n", (unsigned long)pthread_self(), reponse);

            // ***** Décision de fermeture **************
            if (status == FERMER_CONNEXION)
            {
                printf("\t[THREAD %lu] Fin de connexion de la socket %d\n", 
                       (unsigned long)pthread_self(), sService);
                close(sService);
                return;
            }
        }
}

// Fonction de chargement de la configuration
/**
 * Charge la configuration du serveur depuis un fichier
 * 
 * @param nomFichier: Nom du fichier de configuration à charger
 * 
 * @return int: 
 *   - 0: Chargement réussi
 *   - -1: Erreur lors du chargement
 * 
 * Cette fonction sert à:
 * - Lire le fichier de configuration
 * - Parser les paramètres (port, threads, file d'attente)
 * - Initialiser les variables globales
 * - Valider la configuration
 * 
 * Note: Format du fichier: "cle=valeur" (une par ligne)
 */
int chargerConfiguration(const char* nomFichier)
{
    FILE* fichier = fopen(nomFichier, "r");
    if (fichier == NULL)
    {
        printf("Impossible d'ouvrir le fichier de configuration: %s\n", nomFichier);
        return -1;
    }
    
    char ligne[256];
    char cle[64], valeur[64];
    
    while (fgets(ligne, sizeof(ligne), fichier) != NULL)
    {
        // Ignorer les commentaires et lignes vides
        if (ligne[0] == '#' || ligne[0] == '\n' || ligne[0] == '\r')
            continue;
            
        // Parser la ligne "cle=valeur"
        if (sscanf(ligne, "%63[^=]=%63s", cle, valeur) == 2)
        {
            if (strcmp(cle, "PORT_RESERVATION") == 0)
            {
                PORT_RESERVATION = atoi(valeur);
            }
            else if (strcmp(cle, "NB_THREADS_POOL") == 0)
            {
                NB_THREADS_POOL = atoi(valeur);
            }
            else if (strcmp(cle, "TAILLE_FILE_ATTENTE") == 0)
            {
                TAILLE_FILE_ATTENTE = atoi(valeur);
            }
        }
    }
    
    fclose(fichier);
    return 0;
}