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

typedef struct {
    int socket;
    int patientId;
    char nom[MAX_NAME_LEN];
    char prenom[MAX_NAME_LEN];
    char ip[IP_STR_LEN];
} CLIENT_INFO;

CLIENT_INFO clients[NB_MAX_CLIENTS];
int nbClients = 0;

int estPresent(int socket);
void ajoute(int socket, int patientId, const char* nom, const char* prenom);
void retire(int socket);

pthread_mutex_t mutexClients = PTHREAD_MUTEX_INITIALIZER;

MYSQL* connexionBD = NULL;
pthread_mutex_t mutexBD = PTHREAD_MUTEX_INITIALIZER;

int connecterBD();
void deconnecterBD(); 
bool verifierAuthentification(int socket, const char* commande, char* reponse)
{
    if (estPresent(socket) == -1) {
        sprintf(reponse, "%s#%s#Client non loggé !", commande, KO);
        return false;
    }
    return true;
}

bool executerRequeteBD(const char* commande, const char* requeteSQL, char* reponse, char* temp)
{
    pthread_mutex_lock(&mutexBD);
    if (connexionBD == NULL && connecterBD() != 0) {
        pthread_mutex_unlock(&mutexBD);
        sprintf(reponse, "%s#%s#Erreur connexion BD !", commande, KO);
        return false;
    }
    
    if (mysql_query(connexionBD, requeteSQL)) {
        pthread_mutex_unlock(&mutexBD);
        sprintf(reponse, "%s#%s#Erreur requête BD !", commande, KO);
        return false;
    }
    
    MYSQL_RES* resultat = mysql_store_result(connexionBD);
    if (resultat) {
        strcpy(temp, commande);
        strcat(temp, "#" OK "#");
        
        MYSQL_ROW ligne;
        int first = 1;
        while ((ligne = mysql_fetch_row(resultat))) {
            char buffer[MED_BUF];
            int cols = mysql_num_fields(resultat);
            
            sprintf(buffer, "%s%s", first ? "#" : "|", ligne[0]);
            for (int i = 1; i < cols; i++) {
                strcat(buffer, "#");
                strcat(buffer, ligne[i] ? ligne[i] : "");
            }
            strcat(temp, buffer);
            first = 0;
        }
        mysql_free_result(resultat);
    }
    pthread_mutex_unlock(&mutexBD);
    strcpy(reponse, temp);
    return true;
}

bool CBP(char* requete, char* reponse, int socket)
{
    char *ptr = strtok(requete, "#");
    
    if (strcmp(ptr, LOGIN) == 0) {
        char nom[MAX_NAME_LEN], prenom[MAX_NAME_LEN], numeroPatientStr[MAX_ID_LEN], nouveauPatientStr[FLAG_LEN];
        int numeroPatient, nouveauPatient, patientId;
        
        strcpy(nom, strtok(NULL, "#"));
        strcpy(prenom, strtok(NULL, "#"));
        strcpy(numeroPatientStr, strtok(NULL, "#"));
        strcpy(nouveauPatientStr, strtok(NULL, "#"));
        
        numeroPatient = atoi(numeroPatientStr);
        nouveauPatient = atoi(nouveauPatientStr);
        
        printf("\t[THREAD %lu] LOGIN de %s %s (nouveau: %s)\n", 
               (unsigned long)pthread_self(), nom, prenom, nouveauPatient ? "OUI" : "NON");
        
        if (estPresent(socket) >= 0) {
            sprintf(reponse, LOGIN "#" KO "#Client déjà loggé !");
            return false;
        }
        
        int resultatLogin = CBP_Login(nom, prenom, numeroPatient, nouveauPatient, &patientId);
        
        switch(resultatLogin) {
            case SUCCES:
                if (nouveauPatient) {
                    sprintf(reponse, LOGIN "#" OK "#%d", patientId);
                } else {
                    sprintf(reponse, LOGIN "#" OK "#%d", patientId);
                }
                ajoute(socket, patientId, nom, prenom);
                break;
                
            case PATIENT_NON_TROUVE:
                sprintf(reponse, LOGIN "#" KO "#Patient non trouvé !");
                return false;
                
            case ERREUR_BD:
                sprintf(reponse, LOGIN "#" KO "#Erreur base de données !");
                return false;
                
            default:
                sprintf(reponse, LOGIN "#" KO "#Erreur d'authentification !");
                return false;
        }
    }
    
    else if (strcmp(ptr, LOGOUT) == 0) {
        printf("\t[THREAD %lu] LOGOUT\n", (unsigned long)pthread_self());
        retire(socket);
        sprintf(reponse, LOGOUT "#" OK "#");
        return false;
    }
    
    else if (strcmp(ptr, GET_SPECIALTIES) == 0) {
        printf("\t[THREAD %lu] GET_SPECIALTIES\n", (unsigned long)pthread_self());
        if (!verifierAuthentification(socket, GET_SPECIALTIES, reponse)) return true;
        
        char temp[BIG_BUF];
        executerRequeteBD(GET_SPECIALTIES, "SELECT id, name FROM specialties ORDER BY name", reponse, temp);
    }
    
    else if (strcmp(ptr, GET_DOCTORS) == 0) {
        printf("\t[THREAD %lu] GET_DOCTORS\n", (unsigned long)pthread_self());
        if (!verifierAuthentification(socket, GET_DOCTORS, reponse)) return true;
        
        char temp[BIG_BUF];
        executerRequeteBD(GET_DOCTORS, 
            "SELECT d.id, CONCAT(d.last_name, ' ', d.first_name), s.name FROM doctors d "
            "JOIN specialties s ON d.specialty_id = s.id ORDER BY d.last_name, d.first_name", 
            reponse, temp);
    }
    
    else if (strcmp(ptr, SEARCH_CONSULTATIONS) == 0) {
        char specialty[MAX_NAME_LEN], doctor[MAX_NAME_LEN], startDate[20], endDate[20];
        strcpy(specialty, strtok(NULL, "#"));
        strcpy(doctor, strtok(NULL, "#"));
        strcpy(startDate, strtok(NULL, "#"));
        strcpy(endDate, strtok(NULL, "#"));
        
        printf("\t[THREAD %lu] SEARCH_CONSULTATIONS\n", (unsigned long)pthread_self());
        if (!verifierAuthentification(socket, SEARCH_CONSULTATIONS, reponse)) return true;
        
        char requeteSQL[BIG_BUF], temp[HUGE_BUF];
        sprintf(requeteSQL, 
            "SELECT c.id, CONCAT(d.last_name, ' ', d.first_name), s.name, c.date, c.hour "
            "FROM consultations c JOIN doctors d ON c.doctor_id = d.id "
            "JOIN specialties s ON d.specialty_id = s.id WHERE c.patient_id IS NULL "
            "AND ('%s' = '" TOUTES "' OR s.name = '%s') "
            "AND ('%s' = '" TOUS "' OR CONCAT(d.last_name, ' ', d.first_name) = '%s') "
            "AND c.date >= '%s' AND c.date <= '%s' ORDER BY c.date, c.hour",
            specialty, specialty, doctor, doctor, startDate, endDate);
        
        executerRequeteBD(SEARCH_CONSULTATIONS, requeteSQL, reponse, temp);
    }
    
    else if (strcmp(ptr, BOOK_CONSULTATION) == 0) {
        char consultationId[MAX_ID_LEN], reason[SMALL_BUF];
        strcpy(consultationId, strtok(NULL, "#"));
        strcpy(reason, strtok(NULL, "#"));
        
        printf("\t[THREAD %lu] BOOK_CONSULTATION\n", (unsigned long)pthread_self());
        if (!verifierAuthentification(socket, BOOK_CONSULTATION, reponse)) return true;
        
        // Récupérer l'ID du patient connecté
        int indexClient = estPresent(socket);
        if (indexClient == -1) {
            sprintf(reponse, BOOK_CONSULTATION "#" KO "#Patient non trouvé !");
            return true;
        }
        
        int patientId = clients[indexClient].patientId;
        
        pthread_mutex_lock(&mutexBD);
        if (connexionBD == NULL && connecterBD() != 0) {
            pthread_mutex_unlock(&mutexBD);
            sprintf(reponse, BOOK_CONSULTATION "#" KO "#Erreur connexion BD !");
        } else {
            char requeteSQL[BIG_BUF];
            sprintf(requeteSQL, 
                "UPDATE consultations SET patient_id = %d, reason = '%s' WHERE id = %s AND patient_id IS NULL",
                patientId, reason, consultationId);
            
            if (mysql_query(connexionBD, requeteSQL)) {
                sprintf(reponse, BOOK_CONSULTATION "#" KO "#Erreur requête BD !");
            } else {
                sprintf(reponse, BOOK_CONSULTATION "#%s#%s", 
                    mysql_affected_rows(connexionBD) > 0 ? OK : KO,
                    mysql_affected_rows(connexionBD) > 0 ? "Consultation réservée !" : "Consultation non disponible !");
            }
            pthread_mutex_unlock(&mutexBD);
        }
    }
    
    return true;
} 
int CBP_Login(const char* nom, const char* prenom, int numeroPatient, int nouveauPatient, int* patientId)
{
    pthread_mutex_lock(&mutexBD);
    
    if (connexionBD == NULL && connecterBD() != 0) {
        pthread_mutex_unlock(&mutexBD);
        return ERREUR_BD;
    }
    
    char requeteSQL[BIG_BUF];
    MYSQL_RES* resultat = NULL;
    MYSQL_ROW ligne;
    
    if (nouveauPatient) {
        sprintf(requeteSQL, 
            "INSERT INTO patients (last_name, first_name, birth_date) VALUES ('%s', '%s', '1900-01-01')", 
            nom, prenom);
        
        if (mysql_query(connexionBD, requeteSQL)) {
            pthread_mutex_unlock(&mutexBD);
            return ERREUR_BD;
        }
        
        // Récupérer l'ID généré
        *patientId = (int)mysql_insert_id(connexionBD);
        pthread_mutex_unlock(&mutexBD);
        return SUCCES;
    } else {
        sprintf(requeteSQL, 
            "SELECT id FROM patients WHERE last_name = '%s' AND first_name = '%s' AND id = %d", 
            nom, prenom, numeroPatient);
        
        if (mysql_query(connexionBD, requeteSQL)) {
            pthread_mutex_unlock(&mutexBD);
            return ERREUR_BD;
        }
        
        resultat = mysql_store_result(connexionBD);
        if (resultat) {
            if ((ligne = mysql_fetch_row(resultat))) {
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

int CBP_Operation(char op, int a, int b)
{
    if (op == '+') return a + b;
    if (op == '-') return a - b;
    if (op == '*') return a * b;
    if (op == '/')
    {
        if (b == 0) throw 1;
        return a / b;
    }
    return 0;
}

int estPresent(int socket)
{
    int indice = -1;
    pthread_mutex_lock(&mutexClients);
    for (int i = 0; i < nbClients; i++)
        if (clients[i].socket == socket) { indice = i; break; }
    pthread_mutex_unlock(&mutexClients);
    return indice;
}

void ajoute(int socket, int patientId, const char* nom, const char* prenom)
{
    pthread_mutex_lock(&mutexClients);
    clients[nbClients].socket = socket;
    clients[nbClients].patientId = patientId;
    strcpy(clients[nbClients].nom, nom);
    strcpy(clients[nbClients].prenom, prenom);
    strcpy(clients[nbClients].ip, "127.0.0.1"); // TODO: récupérer vraie IP
    nbClients++;
    pthread_mutex_unlock(&mutexClients);
}

void retire(int socket)
{
    int pos = estPresent(socket);
    if (pos == -1) return;
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
        return 0; // Déjà connecté
        
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

int CBP_GetPatientsConnectes(char* buffer, int tailleBuff)
{
    pthread_mutex_lock(&mutexClients);
    
    strcpy(buffer, "");
    for (int i = 0; i < nbClients; i++) {
        char lignePatient[MED_BUF];
        sprintf(lignePatient, "%s%d#%s#%s#%s", 
                (i == 0) ? "" : "|",
                clients[i].patientId,
                clients[i].nom,
                clients[i].prenom,
                clients[i].ip);
        
        if (strlen(buffer) + strlen(lignePatient) < (size_t)(tailleBuff - 1)) {
            strcat(buffer, lignePatient);
        }
    }
    
    int nb = nbClients;
    pthread_mutex_unlock(&mutexClients);
    return nb;
} 