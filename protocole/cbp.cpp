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

//***** Etat du protocole : liste des clients loggés ****************
int clients[NB_MAX_CLIENTS];
int nbClients = 0;

int estPresent(int socket);
void ajoute(int socket);
void retire(int socket);

pthread_mutex_t mutexClients = PTHREAD_MUTEX_INITIALIZER;

//***** Connexion à la base de données ******************************
MYSQL* connexionBD = NULL;
pthread_mutex_t mutexBD = PTHREAD_MUTEX_INITIALIZER;

int connecterBD();
void deconnecterBD(); 
//***** Parsing de la requete et creation de la reponse *************
bool CBP(char* requete, char* reponse, int socket)
{
    // ***** Récupération nom de la requete *****************
    char *ptr = strtok(requete, "#");
    
    // ***** LOGIN ******************************************
    if (strcmp(ptr, LOGIN) == 0)
    {
        char user[50], password[50];
        strcpy(user, strtok(NULL, "#"));
        strcpy(password, strtok(NULL, "#"));
        printf("\t[THREAD %lu] LOGIN de %s\n", (unsigned long)pthread_self(), user);
        
        if (estPresent(socket) >= 0)  // client déjà loggé
        {
            sprintf(reponse, LOGIN "#" KO "#Client déjà loggé !");
            return false;
        }
        else
        {
            if (CBP_Login(user, password))
            {
                sprintf(reponse, LOGIN "#" OK "#"); // TODO: ajouter le numéro de patient
                ajoute(socket);
            }
            else
            {
                sprintf(reponse, LOGIN "#" KO "#Mauvais identifiants !");
                return false;
            }
        }
    }
    
    // ***** LOGOUT *****************************************
    if (strcmp(ptr, LOGOUT) == 0)
    {
        printf("\t[THREAD %lu] LOGOUT\n", (unsigned long)pthread_self());
        retire(socket);
        sprintf(reponse, LOGOUT "#" OK "#");
        return false;
    }
    
    // ***** GET_SPECIALTIES ********************************
    if (strcmp(ptr, GET_SPECIALTIES) == 0)
    {
        printf("\t[THREAD %lu] GET_SPECIALTIES\n", (unsigned long)pthread_self());
        
        if (estPresent(socket) == -1)
        {
            sprintf(reponse, GET_SPECIALTIES "#" KO "#Client non loggé !");
        }
        else
        {
            // Récupérer les spécialités depuis la BD
            pthread_mutex_lock(&mutexBD);
            if (connexionBD == NULL && connecterBD() != 0)
            {
                printf("Erreur de connexion à la base de données pour GET_SPECIALTIES\n");
                pthread_mutex_unlock(&mutexBD);
                sprintf(reponse, GET_SPECIALTIES "#" KO "#Erreur connexion BD !");
            }
            else
            {
                MYSQL_RES* resultat = NULL;
                MYSQL_ROW ligne;
                char temp[1024] = "";
                
                printf("Requête SQL: SELECT id, name FROM specialties ORDER BY name\n");
                if (mysql_query(connexionBD, "SELECT id, name FROM specialties ORDER BY name"))
                {
                    printf("Erreur MySQL: %s\n", mysql_error(connexionBD));
                    pthread_mutex_unlock(&mutexBD);
                    sprintf(reponse, GET_SPECIALTIES "#" KO "#Erreur requête BD: %s", mysql_error(connexionBD));
                }
                else
                {
                    resultat = mysql_store_result(connexionBD);
                    if (resultat)
                    {
                        strcpy(temp, GET_SPECIALTIES "#" OK "#");
                        int first = 1;
                        while ((ligne = mysql_fetch_row(resultat)))
                        {
                            char ligneSpecialite[100];
                            if (first) {
                                sprintf(ligneSpecialite, "#%s#%s", ligne[0], ligne[1]);
                                first = 0;
                            } else {
                                sprintf(ligneSpecialite, "|%s#%s", ligne[0], ligne[1]);
                            }
                            strcat(temp, ligneSpecialite);
                        }
                        mysql_free_result(resultat);
                    }
                    pthread_mutex_unlock(&mutexBD);
                    strcpy(reponse, temp);
                }
            }
        }
    }
    
    // ***** GET_DOCTORS ************************************
    if (strcmp(ptr, GET_DOCTORS) == 0)
    {
        printf("\t[THREAD %lu] GET_DOCTORS\n", (unsigned long)pthread_self());
        
        if (estPresent(socket) == -1)
        {
            sprintf(reponse, GET_DOCTORS "#" KO "#Client non loggé !");
        }
        else
        {
            // Récupérer les médecins depuis la BD
            pthread_mutex_lock(&mutexBD);
            if (connexionBD == NULL && connecterBD() != 0)
            {
                printf("Erreur de connexion à la base de données pour GET_DOCTORS\n");
                pthread_mutex_unlock(&mutexBD);
                sprintf(reponse, GET_DOCTORS "#" KO "#Erreur connexion BD !");
            }
            else
            {
                MYSQL_RES* resultat = NULL;
                MYSQL_ROW ligne;
                char temp[1024] = "";
                
                printf("Requête SQL: SELECT d.id, d.last_name, d.first_name, s.name FROM doctors d JOIN specialties s ON d.specialty_id = s.id ORDER BY d.last_name, d.first_name\n");
                if (mysql_query(connexionBD, "SELECT d.id, d.last_name, d.first_name, s.name FROM doctors d JOIN specialties s ON d.specialty_id = s.id ORDER BY d.last_name, d.first_name"))
                {
                    printf("Erreur MySQL: %s\n", mysql_error(connexionBD));
                    pthread_mutex_unlock(&mutexBD);
                    sprintf(reponse, GET_DOCTORS "#" KO "#Erreur requête BD: %s", mysql_error(connexionBD));
                }
                else
                {
                    resultat = mysql_store_result(connexionBD);
                    if (resultat)
                    {
                        strcpy(temp, GET_DOCTORS "#" OK "#");
                        int first = 1;
                        while ((ligne = mysql_fetch_row(resultat)))
                        {
                            char ligneDocteur[200];
                            if (first) {
                                sprintf(ligneDocteur, "#%s#%s %s#%s", ligne[0], ligne[1], ligne[2], ligne[3]);
                                first = 0;
                            } else {
                                sprintf(ligneDocteur, "|%s#%s %s#%s", ligne[0], ligne[1], ligne[2], ligne[3]);
                            }
                            strcat(temp, ligneDocteur);
                        }
                        mysql_free_result(resultat);
                    }
                    pthread_mutex_unlock(&mutexBD);
                    strcpy(reponse, temp);
                }
            }
        }
    }
    
    // ***** SEARCH_CONSULTATIONS ***************************
    if (strcmp(ptr, SEARCH_CONSULTATIONS) == 0)
    {
        char specialty[50], doctor[50], startDate[20], endDate[20];
        strcpy(specialty, strtok(NULL, "#"));
        strcpy(doctor, strtok(NULL, "#"));
        strcpy(startDate, strtok(NULL, "#"));
        strcpy(endDate, strtok(NULL, "#"));
        
        printf("\t[THREAD %lu] SEARCH_CONSULTATIONS: '%s', '%s', '%s', '%s'\n", 
               (unsigned long)pthread_self(), specialty, doctor, startDate, endDate);
        printf("\t[DEBUG] Longueurs: specialty=%lu, doctor=%lu, startDate=%lu, endDate=%lu\n",
               strlen(specialty), strlen(doctor), strlen(startDate), strlen(endDate));
        
        if (estPresent(socket) == -1)
        {
            sprintf(reponse, SEARCH_CONSULTATIONS "#" KO "#Client non loggé !");
        }
        else
        {
            // Rechercher les consultations disponibles
            pthread_mutex_lock(&mutexBD);
            if (connexionBD == NULL && connecterBD() != 0)
            {
                printf("Erreur de connexion à la base de données\n");
                pthread_mutex_unlock(&mutexBD);
                sprintf(reponse, SEARCH_CONSULTATIONS "#" KO "#Erreur connexion BD !");
            }
            else
            {
                MYSQL_RES* resultat = NULL;
                MYSQL_ROW ligne;
                char temp[2048] = "";
                char requete[1024];
                
                // Construction de la requête SQL
                if (strcmp(specialty, "--- TOUTES ---") == 0 && strcmp(doctor, "--- TOUS ---") == 0)
                {
                    // Cas où on cherche toutes les spécialités et tous les docteurs
                    sprintf(requete, 
                        "SELECT c.id, CONCAT(d.last_name, ' ', d.first_name) as docteur, s.name as specialite, c.date, c.hour "
                        "FROM consultations c "
                        "JOIN doctors d ON c.doctor_id = d.id "
                        "JOIN specialties s ON d.specialty_id = s.id "
                        "WHERE c.patient_id IS NULL "
                        "AND c.date >= '%s' AND c.date <= '%s' "
                        "ORDER BY c.date, c.hour",
                        startDate, endDate);
                }
                else if (strcmp(specialty, "--- TOUTES ---") == 0)
                {
                    // Cas où on cherche toutes les spécialités mais un docteur spécifique
                    sprintf(requete, 
                        "SELECT c.id, CONCAT(d.last_name, ' ', d.first_name) as docteur, s.name as specialite, c.date, c.hour "
                        "FROM consultations c "
                        "JOIN doctors d ON c.doctor_id = d.id "
                        "JOIN specialties s ON d.specialty_id = s.id "
                        "WHERE c.patient_id IS NULL "
                        "AND CONCAT(d.last_name, ' ', d.first_name) = '%s' "
                        "AND c.date >= '%s' AND c.date <= '%s' "
                        "ORDER BY c.date, c.hour",
                        doctor, startDate, endDate);
                }
                else if (strcmp(doctor, "--- TOUS ---") == 0)
                {
                    // Cas où on cherche une spécialité spécifique mais tous les docteurs
                    sprintf(requete, 
                        "SELECT c.id, CONCAT(d.last_name, ' ', d.first_name) as docteur, s.name as specialite, c.date, c.hour "
                        "FROM consultations c "
                        "JOIN doctors d ON c.doctor_id = d.id "
                        "JOIN specialties s ON d.specialty_id = s.id "
                        "WHERE c.patient_id IS NULL "
                        "AND s.name = '%s' "
                        "AND c.date >= '%s' AND c.date <= '%s' "
                        "ORDER BY c.date, c.hour",
                        specialty, startDate, endDate);
                }
                else
                {
                    // Cas où on cherche une spécialité et un docteur spécifiques
                    sprintf(requete, 
                        "SELECT c.id, CONCAT(d.last_name, ' ', d.first_name) as docteur, s.name as specialite, c.date, c.hour "
                        "FROM consultations c "
                        "JOIN doctors d ON c.doctor_id = d.id "
                        "JOIN specialties s ON d.specialty_id = s.id "
                        "WHERE c.patient_id IS NULL "
                        "AND s.name = '%s' "
                        "AND CONCAT(d.last_name, ' ', d.first_name) = '%s' "
                        "AND c.date >= '%s' AND c.date <= '%s' "
                        "ORDER BY c.date, c.hour",
                        specialty, doctor, startDate, endDate);
                }
                
                printf("Requête SQL: %s\n", requete);
                if (mysql_query(connexionBD, requete))
                {
                    printf("Erreur MySQL: %s\n", mysql_error(connexionBD));
                    pthread_mutex_unlock(&mutexBD);
                    sprintf(reponse, SEARCH_CONSULTATIONS "#" KO "#Erreur requête BD: %s", mysql_error(connexionBD));
                }
                else
                {
                    resultat = mysql_store_result(connexionBD);
                    if (resultat)
                    {
                        strcpy(temp, SEARCH_CONSULTATIONS "#" OK "#");
                        int first = 1;
                        while ((ligne = mysql_fetch_row(resultat)))
                        {
                            char ligneConsultation[200];
                            if (first) {
                                sprintf(ligneConsultation, "#%s#%s#%s#%s#%s", 
                                       ligne[0], ligne[1], ligne[2], ligne[3], ligne[4]);
                                first = 0;
                            } else {
                                sprintf(ligneConsultation, "|%s#%s#%s#%s#%s", 
                                       ligne[0], ligne[1], ligne[2], ligne[3], ligne[4]);
                            }
                            strcat(temp, ligneConsultation);
                        }
                        mysql_free_result(resultat);
                    }
                    pthread_mutex_unlock(&mutexBD);
                    strcpy(reponse, temp);
                }
            }
        }
    }
    
    // ***** BOOK_CONSULTATION ******************************
    if (strcmp(ptr, BOOK_CONSULTATION) == 0)
    {
        char consultationId[10], reason[100];
        strcpy(consultationId, strtok(NULL, "#"));
        strcpy(reason, strtok(NULL, "#"));
        
        printf("\t[THREAD %lu] BOOK_CONSULTATION: %s, %s\n",
               (unsigned long)pthread_self(), consultationId, reason);
        
        if (estPresent(socket) == -1)
        {
            sprintf(reponse, BOOK_CONSULTATION "#" KO "#Client non loggé !");
        }
        else
        {
            // Récupérer l'ID du patient connecté
            int patientId = -1;
            pthread_mutex_lock(&mutexClients);
            for (int i = 0; i < nbClients; i++)
            {
                if (clients[i] == socket)
                {
                    patientId = i + 1; // ID patient basé sur l'index
                    break;
                }
            }
            pthread_mutex_unlock(&mutexClients);
            
            if (patientId == -1)
            {
                sprintf(reponse, BOOK_CONSULTATION "#" KO "#Patient non trouvé !");
            }
            else
            {
                // Réserver la consultation
                pthread_mutex_lock(&mutexBD);
                if (connexionBD == NULL && connecterBD() != 0)
                {
                    pthread_mutex_unlock(&mutexBD);
                    sprintf(reponse, BOOK_CONSULTATION "#" KO "#Erreur connexion BD !");
                }
                else
                {
                    char requete[1024];
                    sprintf(requete, 
                        "UPDATE consultations SET patient_id = %d, reason = '%s' WHERE id = %s AND patient_id IS NULL",
                        patientId, reason, consultationId);
                    
                    printf("Requête SQL: %s\n", requete);
                    if (mysql_query(connexionBD, requete))
                    {
                        printf("Erreur MySQL: %s\n", mysql_error(connexionBD));
                        pthread_mutex_unlock(&mutexBD);
                        sprintf(reponse, BOOK_CONSULTATION "#" KO "#Erreur requête BD: %s", mysql_error(connexionBD));
                    }
                    else
                    {
                        if (mysql_affected_rows(connexionBD) > 0)
                        {
                            pthread_mutex_unlock(&mutexBD);
                            sprintf(reponse, BOOK_CONSULTATION "#" OK "#Consultation réservée avec succès !");
                        }
                        else
                        {
                            pthread_mutex_unlock(&mutexBD);
                            sprintf(reponse, BOOK_CONSULTATION "#" KO "#Consultation non disponible !");
                        }
                    }
                }
            }
        }
    }
    
    return true;
} 
//***** Traitement des requetes *************************************
bool CBP_Login(const char* user, const char* password)
{
    if (strcmp(user, "wagner") == 0 && strcmp(password, "abc123") == 0) return true;
    if (strcmp(user, "charlet") == 0 && strcmp(password, "xyz456") == 0) return true;
    return false;
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

//***** Gestion de l'état du protocole ******************************
int estPresent(int socket)
{
    int indice = -1;
    pthread_mutex_lock(&mutexClients);
    for (int i = 0; i < nbClients; i++)
        if (clients[i] == socket) { indice = i; break; }
    pthread_mutex_unlock(&mutexClients);
    return indice;
}

void ajoute(int socket)
{
    pthread_mutex_lock(&mutexClients);
    clients[nbClients] = socket;
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

//***** Fin prématurée **********************************************
void CBP_Close()
{
    pthread_mutex_lock(&mutexClients);
    for (int i = 0; i < nbClients; i++)
        close(clients[i]);
    pthread_mutex_unlock(&mutexClients);
    
    deconnecterBD();
}

//***** Fonctions de connexion à la base de données ****************
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