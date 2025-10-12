#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include "../TCP/TCP.h"
#include "../protocole/cbp.h"
#include "../util/const.h"

void HandlerSIGINT(int s);

void TraitementConnexion(int sService);

void* FctThreadClient(void* p);

int chargerConfiguration(const char* nomFichier);

int PORT_RESERVATION;
int NB_THREADS_POOL;
int TAILLE_FILE_ATTENTE;

int sEcoute;

int* socketsAcceptees;
int indiceEcriture = 0, indiceLecture = 0;
pthread_mutex_t mutexSocketsAcceptees;
pthread_cond_t condSocketsAcceptees; 
int main(int argc, char* argv[])
{
    const char* fichierConfig = "serveur.conf";
    
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
    
    printf("Tentative de création du socket sur le port %d...\n", PORT_RESERVATION);
    if ((sEcoute = ServerSocket(PORT_RESERVATION)) == -1)
    {
        perror("Erreur de ServeurSocket");
        printf("Impossible de créer le socket sur le port %d\n", PORT_RESERVATION);
        exit(1);
    }
    printf("Socket créé avec succès sur le port %d\n", PORT_RESERVATION);
    
    printf("Création du pool de threads.\n");
    pthread_t th;
    for (int i = 0; i < NB_THREADS_POOL; i++)
        pthread_create(&th, NULL, FctThreadClient, NULL);
    
    int sService;
    char ipClient[IP_STR_LEN] = DEFAULT_SERVER_IP;
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
        
        pthread_mutex_lock(&mutexSocketsAcceptees);
        socketsAcceptees[indiceEcriture] = sService;
        indiceEcriture++;
        if (indiceEcriture == TAILLE_FILE_ATTENTE) indiceEcriture = 0;
        pthread_mutex_unlock(&mutexSocketsAcceptees);
        pthread_cond_signal(&condSocketsAcceptees);
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

void HandlerSIGINT(int s)
{
    (void)s;
    printf("\nArret du serveur.\n");
    close(sEcoute);
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