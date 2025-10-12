#include "cbp.h"
#include "../util/const.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <mysql.h>
#include <cstddef>
#include <cstring>

typedef struct
{
    int socket;
    int patientId;
    char nom[MAX_NAME_LEN];
    char prenom[MAX_NAME_LEN];
    char ip[IP_STR_LEN];
} CLIENT_INFO;

CLIENT_INFO clients[NB_MAX_CLIENTS];
int nbClients = 0;

int estPresent(int socket);
void ajoute(int socket, int patientId, const char *nom, const char *prenom);
void retire(int socket);

pthread_mutex_t mutexClients = PTHREAD_MUTEX_INITIALIZER;

MYSQL *connexionBD = NULL;
pthread_mutex_t mutexBD = PTHREAD_MUTEX_INITIALIZER;

int connecterBD();
void deconnecterBD();

void formatErrorResponse(char* reponse, const char* commande, const char* message);
void formatSuccessResponse(char* reponse, const char* commande, const char* data);
bool handleLogin(char* params, char* reponse, int socket);
bool handleLogout(char* reponse, int socket);
bool handleGetSpecialties(char* reponse, int socket);
bool handleGetDoctors(char* reponse, int socket);
bool handleSearchConsultations(char* params, char* reponse, int socket);
bool handleBookConsultation(char* params, char* reponse, int socket);
bool verifierAuthentification(int socket, const char *commande, char *reponse)
{
    if (estPresent(socket) == -1)
    {
        sprintf(reponse, "%s#%s#Client non loggé !", commande, KO);
        return false;
    }
    return true;
}

bool executerRequeteBD(const char *commande, const char *requeteSQL, char *reponse, char *temp)
{
    pthread_mutex_lock(&mutexBD);
    if (connexionBD == NULL && connecterBD() != 0)
    {
        pthread_mutex_unlock(&mutexBD);
        formatErrorResponse(reponse, commande, "Erreur connexion BD !");
        return false;
    }

    if (mysql_query(connexionBD, requeteSQL))
    {
        pthread_mutex_unlock(&mutexBD);
        formatErrorResponse(reponse, commande, "Erreur requête BD !");
        return false;
    }

    MYSQL_RES *resultat = mysql_store_result(connexionBD);
    if (resultat)
    {
        strcpy(temp, commande);
        strcat(temp, "#" OK "#");

        MYSQL_ROW ligne;
        bool first = true;
        while ((ligne = mysql_fetch_row(resultat)))
        {
            int cols = mysql_num_fields(resultat);

            if (first) {
                strcat(temp, "#");
                first = false;
            } else {
                strcat(temp, "|");
            }
            
            strcat(temp, ligne[0]);
            
            for (int i = 1; i < cols; i++)
            {
                strcat(temp, "#");
                if (ligne[i]) {
                    strcat(temp, ligne[i]);
                }
            }
        }
        mysql_free_result(resultat);
    }
    pthread_mutex_unlock(&mutexBD);
    strcpy(reponse, temp);
    return true;
}

bool CBP(char *requete, char *reponse, int socket)
{
    char *commande = strtok(requete, "#");
    char *params = strtok(NULL, "");
    
    if (strcmp(commande, LOGIN) == 0) {
        return handleLogin(params, reponse, socket);
    }
    else if (strcmp(commande, LOGOUT) == 0) {
        return handleLogout(reponse, socket);
    }
    else if (strcmp(commande, GET_SPECIALTIES) == 0) {
        return handleGetSpecialties(reponse, socket);
    }
    else if (strcmp(commande, GET_DOCTORS) == 0) {
        return handleGetDoctors(reponse, socket);
    }
    else if (strcmp(commande, SEARCH_CONSULTATIONS) == 0) {
        return handleSearchConsultations(params, reponse, socket);
    }
    else if (strcmp(commande, BOOK_CONSULTATION) == 0) {
        return handleBookConsultation(params, reponse, socket);
    }
    else {
        formatErrorResponse(reponse, commande, "Commande inconnue !");
        return true;
    }
}
int CBP_Login(const char *nom, const char *prenom, int numeroPatient, int nouveauPatient, int *patientId)
{
    pthread_mutex_lock(&mutexBD);

    if (connexionBD == NULL && connecterBD() != 0)
    {
        pthread_mutex_unlock(&mutexBD);
        return ERREUR_BD;
    }

    char requeteSQL[BIG_BUF];
    MYSQL_RES *resultat = NULL;
    MYSQL_ROW ligne;

    if (nouveauPatient)
    {
        sprintf(requeteSQL,
                "INSERT INTO patients (last_name, first_name, birth_date) VALUES ('%s', '%s', '1900-01-01')",
                nom, prenom);

        if (mysql_query(connexionBD, requeteSQL))
        {
            pthread_mutex_unlock(&mutexBD);
            return ERREUR_BD;
        }

        *patientId = (int)mysql_insert_id(connexionBD);
        pthread_mutex_unlock(&mutexBD);
        return SUCCES;
    }
    else
    {
        sprintf(requeteSQL,
                "SELECT id FROM patients WHERE last_name = '%s' AND first_name = '%s' AND id = %d",
                nom, prenom, numeroPatient);

        if (mysql_query(connexionBD, requeteSQL))
        {
            pthread_mutex_unlock(&mutexBD);
            return ERREUR_BD;
        }

        resultat = mysql_store_result(connexionBD);
        if (resultat)
        {
            if ((ligne = mysql_fetch_row(resultat)))
            {
                *patientId = atoi(ligne[0]);
                mysql_free_result(resultat);
                pthread_mutex_unlock(&mutexBD);
                return SUCCES;
            }
            mysql_free_result(resultat);
        }

        pthread_mutex_unlock(&mutexBD);
        return PATIENT_NON_TROUVE;
    }
}

int estPresent(int socket)
{
    int indice = -1;
    pthread_mutex_lock(&mutexClients);
    for (int i = 0; i < nbClients; i++)
        if (clients[i].socket == socket)
        {
            indice = i;
            break;
        }
    pthread_mutex_unlock(&mutexClients);
    return indice;
}

void ajoute(int socket, int patientId, const char *nom, const char *prenom)
{
    pthread_mutex_lock(&mutexClients);
    clients[nbClients].socket = socket;
    clients[nbClients].patientId = patientId;
    strcpy(clients[nbClients].nom, nom);
    strcpy(clients[nbClients].prenom, prenom);
    strcpy(clients[nbClients].ip, DEFAULT_SERVER_IP); 
    nbClients++;
    pthread_mutex_unlock(&mutexClients);
}

void retire(int socket)
{
    int pos = estPresent(socket);
    if (pos == -1)
        return;
    pthread_mutex_lock(&mutexClients);
    for (int i = pos; i <= nbClients - 2; i++)
        clients[i] = clients[i + 1];
    nbClients--;
    pthread_mutex_unlock(&mutexClients);
}

void CBP_Close()
{
    pthread_mutex_lock(&mutexClients);
    for (int i = 0; i < nbClients; i++)
        close(clients[i].socket);
    pthread_mutex_unlock(&mutexClients);

    deconnecterBD();
}

int connecterBD()
{
    if (connexionBD != NULL)
        return 0;

    connexionBD = mysql_init(NULL);
    if (connexionBD == NULL)
    {
        printf("Erreur d'initialisation MySQL\n");
        return -1;
    }

    if (!mysql_real_connect(connexionBD, "localhost", "Student", "PassStudent1_", "PourStudent", 0, NULL, 0))
    {
        printf("Erreur de connexion MySQL: %s\n", mysql_error(connexionBD));
        mysql_close(connexionBD);
        connexionBD = NULL;
        return -1;
    }

    printf("Connexion à la base de données établie\n");
    return 0;
}

void deconnecterBD()
{
    if (connexionBD != NULL)
    {
        mysql_close(connexionBD);
        connexionBD = NULL;
        printf("Connexion à la base de données fermée\n");
    }
}

void formatErrorResponse(char* reponse, const char* commande, const char* message)
{
    sprintf(reponse, "%s#%s#%s", commande, KO, message);
}

void formatSuccessResponse(char* reponse, const char* commande, const char* data)
{
    sprintf(reponse, "%s#%s#%s", commande, OK, data);
}

bool handleLogin(char* params, char* reponse, int socket)
{
    char nom[MAX_NAME_LEN], prenom[MAX_NAME_LEN], numeroPatientStr[MAX_ID_LEN], nouveauPatientStr[FLAG_LEN];
    int numeroPatient, nouveauPatient, patientId;

    char* paramsCopy = strdup(params);
    char* token = strtok(paramsCopy, "#");
        if (token) {
            strcpy(nom, token);
        } else {
            strcpy(nom, "");
        }

        token = strtok(NULL, "#");
        if (token) {
            strcpy(prenom, token);
        } else {
            strcpy(prenom, "");
        }

        token = strtok(NULL, "#");
        if (token) {
            strcpy(numeroPatientStr, token);
        } else {
            strcpy(numeroPatientStr, "1");
        }

        token = strtok(NULL, "#");
        if (token) {
            strcpy(nouveauPatientStr, token);
        } else {
            strcpy(nouveauPatientStr, "0");
        }
    
    free(paramsCopy);

    numeroPatient = atoi(numeroPatientStr);
    nouveauPatient = atoi(nouveauPatientStr);

    printf("\t[THREAD %lu] LOGIN de %s %s (nouveau: %s)\n",
           (unsigned long)pthread_self(), nom, prenom, nouveauPatient ? "OUI" : "NON");

    if (estPresent(socket) >= 0)
    {
        formatErrorResponse(reponse, LOGIN, "Client déjà loggé !");
        return false;
    }

    int resultatLogin = CBP_Login(nom, prenom, numeroPatient, nouveauPatient, &patientId);

    switch (resultatLogin)
    {
    case SUCCES:
        sprintf(reponse, LOGIN "#" OK "#%d", patientId);
        ajoute(socket, patientId, nom, prenom);
        break;

    case PATIENT_NON_TROUVE:
        formatErrorResponse(reponse, LOGIN, "Patient non trouvé !");
        return false;

    case ERREUR_BD:
        formatErrorResponse(reponse, LOGIN, "Erreur base de données !");
        return false;

    default:
        formatErrorResponse(reponse, LOGIN, "Erreur d'authentification !");
        return false;
    }
    
    return true;
}

bool handleLogout(char* reponse, int socket)
{
    printf("\t[THREAD %lu] LOGOUT\n", (unsigned long)pthread_self());
    retire(socket);
    formatSuccessResponse(reponse, LOGOUT, "");
    return false;
}

bool handleGetSpecialties(char* reponse, int socket)
{
    printf("\t[THREAD %lu] GET_SPECIALTIES\n", (unsigned long)pthread_self());
    if (!verifierAuthentification(socket, GET_SPECIALTIES, reponse))
        return true;

    char temp[BIG_BUF];
    const char* SQL_GET_SPECIALTIES = "SELECT name FROM specialties ORDER BY name";
    executerRequeteBD(GET_SPECIALTIES, SQL_GET_SPECIALTIES, reponse, temp);
    return true;
}

bool handleGetDoctors(char* reponse, int socket)
{
    printf("\t[THREAD %lu] GET_DOCTORS\n", (unsigned long)pthread_self());
    if (!verifierAuthentification(socket, GET_DOCTORS, reponse))
        return true;

    char temp[BIG_BUF];
    const char* SQL_GET_DOCTORS = 
        "SELECT d.id, CONCAT(d.last_name, ' ', d.first_name), s.name "
        "FROM doctors d "
        "JOIN specialties s ON d.specialty_id = s.id "
        "ORDER BY d.last_name, d.first_name";
    
    executerRequeteBD(GET_DOCTORS, SQL_GET_DOCTORS, reponse, temp);
    return true;
}

bool handleSearchConsultations(char* params, char* reponse, int socket)
{
    // Déclaration des variables pour stocker les critères de recherche
    char specialty[MAX_NAME_LEN], doctor[MAX_NAME_LEN], startDate[20], endDate[20];
    
    // Création d'une copie des paramètres pour éviter de modifier l'original
    char* paramsCopy = strdup(params);
    
    // Extraction du premier paramètre : la spécialité médicale
    char* token = strtok(paramsCopy, "#");
    if (token) {
        strcpy(specialty, token);  // Copie la spécialité si elle existe
    } else {
        strcpy(specialty, "");     // Utilise une chaîne vide par défaut
    }
    
    // Extraction du deuxième paramètre : le nom du médecin
    token = strtok(NULL, "#");
    if (token) {
        strcpy(doctor, token);     // Copie le nom du médecin si il existe
    } else {
        strcpy(doctor, "");        // Utilise une chaîne vide par défaut
    }
    
    // Extraction du troisième paramètre : la date de début
    token = strtok(NULL, "#");
    if (token) {
        strcpy(startDate, token);  // Copie la date de début si elle existe
    } else {
        strcpy(startDate, "");     // Utilise une chaîne vide par défaut
    }
    
    // Extraction du quatrième paramètre : la date de fin
    token = strtok(NULL, "#");
    if (token) {
        strcpy(endDate, token);    // Copie la date de fin si elle existe
    } else {
        strcpy(endDate, "");       // Utilise une chaîne vide par défaut
    }
    
    // Libération de la mémoire allouée pour la copie des paramètres
    free(paramsCopy);

    // Affichage d'un message de log pour tracer l'exécution
    printf("\t[THREAD %lu] SEARCH_CONSULTATIONS\n", (unsigned long)pthread_self());
    
    // Vérification que le client est bien authentifié
    if (!verifierAuthentification(socket, SEARCH_CONSULTATIONS, reponse))
        return true;  // Retourne true pour maintenir la connexion ouverte

    // Déclaration des buffers pour la requête SQL et la réponse temporaire
    char requeteSQL[BIG_BUF], temp[HUGE_BUF];
    
    // Template de la requête SQL avec des placeholders %s
    const char* SQL_SEARCH_TEMPLATE = 
        "SELECT c.id, CONCAT(d.last_name, ' ', d.first_name), s.name, c.date, c.hour "  // Sélection des colonnes
        "FROM consultations c "                                                           // Table principale
        "JOIN doctors d ON c.doctor_id = d.id "                                         // Jointure avec la table doctors
        "JOIN specialties s ON d.specialty_id = s.id "                                  // Jointure avec la table specialties
        "WHERE c.patient_id IS NULL "                                                   // Consultations libres uniquement
        "AND ('%s' = '" TOUTES "' OR s.name = '%s') "                                  // Filtre optionnel sur la spécialité
        "AND ('%s' = '" TOUS "' OR CONCAT(d.last_name, ' ', d.first_name) = '%s') "    // Filtre optionnel sur le médecin
        "AND c.date >= '%s' AND c.date <= '%s' "                                       // Filtre sur la période
        "ORDER BY c.date, c.hour";                                                     // Tri par date puis heure
    
    // Construction de la requête SQL finale en remplaçant les %s par les valeurs réelles
    sprintf(requeteSQL, SQL_SEARCH_TEMPLATE,
            specialty, specialty, doctor, doctor, startDate, endDate);

    // Exécution de la requête SQL et formatage de la réponse
    executerRequeteBD(SEARCH_CONSULTATIONS, requeteSQL, reponse, temp);
    
    // Retourne true pour maintenir la connexion client ouverte
    return true;
}

bool handleBookConsultation(char* params, char* reponse, int socket)
{
    char consultationId[MAX_ID_LEN], reason[SMALL_BUF];
    
    char* paramsCopy = strdup(params);
    char* token = strtok(paramsCopy, "#");
    strcpy(consultationId, token ? token : "");
    
    token = strtok(NULL, "#");
    strcpy(reason, token ? token : "");
    
    free(paramsCopy);

    printf("\t[THREAD %lu] BOOK_CONSULTATION\n", (unsigned long)pthread_self());
    if (!verifierAuthentification(socket, BOOK_CONSULTATION, reponse))
        return true;

    int indexClient = estPresent(socket);
    if (indexClient == -1)
    {
        formatErrorResponse(reponse, BOOK_CONSULTATION, "Patient non trouvé !");
        return true;
    }

    int patientId = clients[indexClient].patientId;

    pthread_mutex_lock(&mutexBD);
    if (connexionBD == NULL && connecterBD() != 0)
    {
        pthread_mutex_unlock(&mutexBD);
        formatErrorResponse(reponse, BOOK_CONSULTATION, "Erreur connexion BD !");
        return true;
    }

    char requeteSQL[BIG_BUF];
    sprintf(requeteSQL,
            "UPDATE consultations SET patient_id = %d, reason = '%s' WHERE id = %s AND patient_id IS NULL",
            patientId, reason, consultationId);

    if (mysql_query(connexionBD, requeteSQL))
    {
        formatErrorResponse(reponse, BOOK_CONSULTATION, "Erreur requête BD !");
    }
    else
    {
        if (mysql_affected_rows(connexionBD) > 0)
        {
            formatSuccessResponse(reponse, BOOK_CONSULTATION, "Consultation réservée !");
        }
        else
        {
            formatErrorResponse(reponse, BOOK_CONSULTATION, "Consultation non disponible !");
        }
    }
    pthread_mutex_unlock(&mutexBD);
    
    return true;
}