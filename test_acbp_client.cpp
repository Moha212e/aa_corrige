#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TAILLE_MAX 1024

int ClientSocket(const char* ipServeur, int port);
int Send(int sSocket, const char *data, int taille);
int Receive(int sSocket, char *data);
int closeSocket(int sSocket);

int main()
{
    printf("=== Test Client ACBP ===\n");
    
    // Connexion au serveur sur le port admin
    int socket = ClientSocket("127.0.0.1", 8081);
    if (socket == -1) {
        printf("Erreur: Impossible de se connecter au serveur admin\n");
        return 1;
    }
    
    printf("Connexion au serveur admin réussie\n");
    
    // Envoi de la commande LIST_CLIENTS
    const char* commande = "LIST_CLIENTS";
    printf("Envoi de la commande: %s\n", commande);
    
    if (Send(socket, commande, strlen(commande)) < 0) {
        printf("Erreur envoi commande\n");
        closeSocket(socket);
        return 1;
    }
    
    // Réception de la réponse
    char reponse[TAILLE_MAX];
    int nbRecu = Receive(socket, reponse);
    if (nbRecu < 0) {
        printf("Erreur réception réponse\n");
        closeSocket(socket);
        return 1;
    }
    
    printf("Réponse reçue (%d octets): %s\n", nbRecu, reponse);
    
    // Fermeture de la connexion
    closeSocket(socket);
    printf("Connexion fermée\n");
    
    return 0;
}

// Implémentation des fonctions TCP (simplifiées pour le test)
int ClientSocket(const char* ipServeur, int port)
{
    int sSocket;
    struct sockaddr_in adresseServeur;

    sSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (sSocket < 0)
        return -1;

    adresseServeur.sin_family = AF_INET;
    adresseServeur.sin_addr.s_addr = inet_addr(ipServeur);
    adresseServeur.sin_port = htons(port);

    if (connect(sSocket, (struct sockaddr *)&adresseServeur, sizeof(adresseServeur)) < 0)
    {
        close(sSocket);
        return -1;
    }

    return sSocket;
}

int Send(int sSocket, const char *data, int taille)
{
    char message[TAILLE_MAX + 1];
    strncpy(message, data, taille);
    message[taille] = '\n';

    int totalEnvoye = 0;
    int tailleComplete = taille + 1;
    while (totalEnvoye < tailleComplete)
    {
        int envoye = send(sSocket, message + totalEnvoye, tailleComplete - totalEnvoye, 0);
        if (envoye <= 0)
            return -1;
        totalEnvoye += envoye;
    }
    return totalEnvoye;
}

int Receive(int sSocket, char *data)
{
    char buffer[TAILLE_MAX];
    int totalRecu = 0;

    while (totalRecu < TAILLE_MAX)
    {
        int recu = recv(sSocket, buffer + totalRecu, TAILLE_MAX - totalRecu, 0);
        if (recu <= 0)
            return -1;

        totalRecu += recu;

        for (int i = 0; i < totalRecu; i++)
        {
            if (buffer[i] == '\n')
            {
                buffer[i] = '\0';
                strcpy(data, buffer);
                return i;
            }
        }
    }

    return -1;
}

int closeSocket(int sSocket)
{
    if (close(sSocket) < 0)
    {
        return -1;
    }
    return 0;
}
