#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/select.h>
#include "../TCP/TCP.h"
#include "../protocole/cbp.h"
#include "../util/const.h"

void HandlerSIGINT(int s);

void TraitementConnexion(int sService);
void TraitementConnexionAdmin(int sService);

void* FctThreadClient(void* p);
void* FctThreadAdmin(void* p);

void* FctAcceptCBP(void* p);
void* FctAcceptAdmin(void* p);

int chargerConfiguration(const char* nomFichier);

int PORT_RESERVATION;
int PORT_ADMIN;
int NB_THREADS_POOL;
int TAILLE_FILE_ATTENTE;

int sEcoute;
int sEcouteAdmin;

int* socketsAcceptees;
int indiceEcriture = 0, indiceLecture = 0;
pthread_mutex_t mutexSocketsAcceptees;
pthread_cond_t condSocketsAcceptees; 
int main(int argc, char* argv[])
{
    (void)argc; // argc non utilisé
    const char* fichierConfig = "serveur.conf";
    
    printf("Chargement du fichier de configuration : %s\n", fichierConfig);
    if (chargerConfiguration(fichierConfig) != 0)
    {
        printf("Erreur lors du chargement de la configuration\n");
        printf("Usage: %s [fichier_config]\n", argv[0]);
        exit(1);
    }
    
    printf("Configuration chargée depuis le fichier :\n");
    printf("  Port Réservation (CBP): %d\n", PORT_RESERVATION);
    printf("  Port Admin (ACBP): %d\n", PORT_ADMIN);
    printf("  Threads: %d\n", NB_THREADS_POOL);
    printf("  File d'attente: %d\n", 5);
    
    socketsAcceptees = (int*)malloc(TAILLE_FILE_ATTENTE * sizeof(int));
    if (socketsAcceptees == NULL)
    {
        printf("Erreur d'allocation mémoire\n");
        exit(1);
    }

    pthread_mutex_init(&mutexSocketsAcceptees, NULL);
    pthread_cond_init(&condSocketsAcceptees, NULL);
    for (int i = 0; i < TAILLE_FILE_ATTENTE; i++)
        socketsAcceptees[i] = -1;
    
    struct sigaction A;
    A.sa_flags = 0;
    sigemptyset(&A.sa_mask);
    A.sa_handler = HandlerSIGINT;
    if (sigaction(SIGINT, &A, NULL) == -1)
    {
        perror("Erreur de sigaction");
        exit(1);
    }
    
    printf("Tentative de création des sockets...\n");
    
    // Création du socket pour le protocole CBP (patients)
    if ((sEcoute = ServerSocket(PORT_RESERVATION)) == -1)
    {
        perror("Erreur de ServeurSocket (CBP)");
        printf("Impossible de créer le socket sur le port %d\n", PORT_RESERVATION);
        exit(1);
    }
    printf("Socket CBP créé avec succès sur le port %d\n", PORT_RESERVATION);
    
    // Création du socket pour le protocole ACBP (admin)
    if ((sEcouteAdmin = ServerSocket(PORT_ADMIN)) == -1)
    {
        perror("Erreur de ServeurSocket (ACBP)");
        printf("Impossible de créer le socket sur le port %d\n", PORT_ADMIN);
        close(sEcoute);
        exit(1);
    }
    printf("Socket ACBP créé avec succès sur le port %d\n", PORT_ADMIN);
    
    printf("Création du pool de threads.\n");
    pthread_t th;
    for (int i = 0; i < NB_THREADS_POOL; i++)
        pthread_create(&th, NULL, FctThreadClient, NULL);

    // Lancer deux threads d'accept séparés : un pour CBP (clients) et un pour ACBP (admin)
    pthread_t acceptThreadCBP, acceptThreadAdmin;
    if (pthread_create(&acceptThreadCBP, NULL, FctAcceptCBP, NULL) != 0)
    {
        perror("Erreur création thread accept CBP");
        exit(1);
    }
    if (pthread_create(&acceptThreadAdmin, NULL, FctAcceptAdmin, NULL) != 0)
    {
        perror("Erreur création thread accept Admin");
        exit(1);
    }
    pthread_detach(acceptThreadCBP);
    pthread_detach(acceptThreadAdmin);

    printf("Démarrage du serveur (accept threads lancés).\n");

    // Boucle principale légère pour garder le processus actif (les threads font le travail)
    while (1)
    {
        sleep(1);
    }
} 

void* FctThreadClient(void* p)
{
    (void)p;
    int sService;
    while (1)
    {
        printf("\t[THREAD %lu] Attente socket...\n", (unsigned long)pthread_self());
        
        pthread_mutex_lock(&mutexSocketsAcceptees);
        while (indiceEcriture == indiceLecture)
            pthread_cond_wait(&condSocketsAcceptees, &mutexSocketsAcceptees);
        sService = socketsAcceptees[indiceLecture];
        socketsAcceptees[indiceLecture] = -1;
        indiceLecture++;
        if (indiceLecture == TAILLE_FILE_ATTENTE) indiceLecture = 0;
        pthread_mutex_unlock(&mutexSocketsAcceptees);
        
        printf("\t[THREAD %lu] Je m'occupe de la socket %d\n",
               (unsigned long)pthread_self(), sService);
        TraitementConnexion(sService);
    }
}

void* FctThreadAdmin(void* p)
{
    int sService = *(int*)p;
    free(p);
    printf("\t[THREAD ADMIN %lu] Traitement connexion admin socket %d\n",
           (unsigned long)pthread_self(), sService);
    TraitementConnexionAdmin(sService);
    return NULL;
} 

// Thread d'accept pour les clients (CBP)
void* FctAcceptCBP(void* p)
{
    (void)p;
    int sService;
    char ipClient[IP_STR_LEN] = DEFAULT_SERVER_IP;

    while (1)
    {
        if ((sService = Accept(sEcoute, ipClient)) == -1)
        {
            perror("Erreur de Accept (CBP)");
            sleep(1);
            continue;
        }
        printf("Connexion CBP acceptée : IP=%s socket=%d\n", ipClient, sService);

        pthread_mutex_lock(&mutexSocketsAcceptees);
        socketsAcceptees[indiceEcriture] = sService;
        indiceEcriture++;
        if (indiceEcriture == TAILLE_FILE_ATTENTE) indiceEcriture = 0;
        pthread_mutex_unlock(&mutexSocketsAcceptees);
        pthread_cond_signal(&condSocketsAcceptees);
    }
    return NULL;
}

// Thread d'accept pour les admin (ACBP)
void* FctAcceptAdmin(void* p)
{
    (void)p;
    int sService;
    char ipClient[IP_STR_LEN] = DEFAULT_SERVER_IP;

    while (1)
    {
        if ((sService = Accept(sEcouteAdmin, ipClient)) == -1)
        {
            perror("Erreur de Accept (ACBP)");
            sleep(1);
            continue;
        }
        printf("Connexion ACBP acceptée : IP=%s socket=%d\n", ipClient, sService);

        int* pSocket = (int*)malloc(sizeof(int));
        if (pSocket == NULL)
        {
            close(sService);
            continue;
        }
        *pSocket = sService;

        pthread_t threadAdmin;
        pthread_create(&threadAdmin, NULL, FctThreadAdmin, (void*)pSocket);
        pthread_detach(threadAdmin);
    }
    return NULL;
}

void HandlerSIGINT(int s)
{
    (void)s;
    printf("\nArret du serveur.\n");
    close(sEcoute);
    close(sEcouteAdmin);
    pthread_mutex_lock(&mutexSocketsAcceptees);
    for (int i = 0; i < TAILLE_FILE_ATTENTE; i++)
        if (socketsAcceptees[i] != -1) close(socketsAcceptees[i]);
    pthread_mutex_unlock(&mutexSocketsAcceptees);
    CBP_Close();
    exit(0);
} 

void TraitementConnexion(int sService)
{
        char requete[MED_BUF], reponse[MED_BUF];
        int nbLus, nbEcrits;
        int status = SUCCES;

        while (1)
        {
            printf("\t[THREAD %lu] Attente requete...\n", (unsigned long)pthread_self());

            if ((nbLus = Receive(sService, requete)) < 0)
            {
                perror("Erreur de Receive");
                close(sService);
                return;
            }

            if (nbLus == 0)
            {
                printf("\t[THREAD %lu] Fin de connexion du client.\n", (unsigned long)pthread_self());
                close(sService);
                return;
            }

            requete[nbLus] = 0;
            printf("\t[THREAD %lu] Requete recue = %s\n", (unsigned long)pthread_self(), requete);

            status = CBP(requete, reponse, sService);

            if ((nbEcrits = Send(sService, reponse, strlen(reponse))) < 0)
            {
                perror("Erreur de Send");
                close(sService);
                return;
            }

            printf("\t[THREAD %lu] Reponse envoyee = %s\n", (unsigned long)pthread_self(), reponse);

            if (status == FERMER_CONNEXION)
            {
                printf("\t[THREAD %lu] Fin de connexion de la socket %d\n", 
                       (unsigned long)pthread_self(), sService);
                close(sService);
                return;
            }
        }
}

void TraitementConnexionAdmin(int sService)
{
        char requete[MED_BUF], reponse[MED_BUF];
        int nbLus, nbEcrits;
        int status = SUCCES;

        while (1)
        {
            printf("\t[THREAD ADMIN %lu] Attente requete admin...\n", (unsigned long)pthread_self());

            if ((nbLus = Receive(sService, requete)) < 0)
            {
                perror("Erreur de Receive (Admin)");
                close(sService);
                return;
            }

            if (nbLus == 0)
            {
                printf("\t[THREAD ADMIN %lu] Fin de connexion du client admin.\n", (unsigned long)pthread_self());
                close(sService);
                return;
            }

            requete[nbLus] = 0;
            printf("\t[THREAD ADMIN %lu] Requete admin recue = %s\n", (unsigned long)pthread_self(), requete);

            status = ACBP(requete, reponse, sService);

            if ((nbEcrits = Send(sService, reponse, strlen(reponse))) < 0)
            {
                perror("Erreur de Send (Admin)");
                close(sService);
                return;
            }

            printf("\t[THREAD ADMIN %lu] Reponse admin envoyee = %s\n", (unsigned long)pthread_self(), reponse);

            if (status == FERMER_CONNEXION)
            {
                printf("\t[THREAD ADMIN %lu] Fin de connexion de la socket admin %d\n", 
                       (unsigned long)pthread_self(), sService);
                close(sService);
                return;
            }
        }
}

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
        if (ligne[0] == '#' || ligne[0] == '\n' || ligne[0] == '\r')
            continue;
            
        if (sscanf(ligne, "%63[^=]=%63s", cle, valeur) == 2)
        {
            if (strcmp(cle, "PORT_RESERVATION") == 0)
            {
                PORT_RESERVATION = atoi(valeur);
            }
            else if (strcmp(cle, "PORT_ADMIN") == 0)
            {
                PORT_ADMIN = atoi(valeur);
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