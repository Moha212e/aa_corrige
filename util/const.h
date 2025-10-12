#ifndef CONST_H
#define CONST_H

#define LOGIN "LOGIN"
#define LOGOUT "LOGOUT"

#define GET_SPECIALTIES "GET_SPECIALTIES"
#define GET_DOCTORS "GET_DOCTORS"
#define GET_PATIENTS "GET_PATIENTS"
#define GET_CONSULTATIONS "GET_CONSULTATIONS"
#define GET_CONSULTATION "GET_CONSULTATION"

#define SEARCH_CONSULTATIONS "SEARCH_CONSULTATIONS"
#define BOOK_CONSULTATION "BOOK_CONSULTATION"

#define KO "ko"
#define OK "ok"

#define TOUS "--- TOUS ---"
#define TOUTES "--- TOUTES ---"

#define diez "#"
#define pipeSeparator "|"

#define DEFAULT_DATE_FORMAT "yyyy-MM-dd"
#define DEFAULT_DATE_DEBUT "2025-09-15"
#define DEFAULT_DATE_FIN "2025-12-31"

#define NOUVEAU_PATIENT "1"
#define PATIENT_EXISTANT "0"

#define SUCCES 0
#define MAUVAIS_IDENTIFIANTS 1
#define PATIENT_NON_TROUVE 2
#define ERREUR_BD 3
#define ERREUR_INCONNUE 4
#define FERMER_CONNEXION 99

#define MAX_NAME_LEN 50
#define MAX_ID_LEN 10
#define FLAG_LEN 2

#define SMALL_BUF 100
#define MED_BUF 200
#define BIG_BUF 1024
#define HUGE_BUF 2048

#define IP_STR_LEN 50

#define DB_HOST "localhost"
#define DB_USER "Student"
#define DB_PASS "PassStudent1_"
#define DB_NAME "PourStudent"

#define DEFAULT_SERVER_IP "127.1.1.1"
#define DEFAULT_SERVER_PORT 8080

#define COL_COUNT_CONSULTATIONS 5
#define COL_WIDTH_ID 40
#define COL_WIDTH_SPECIALTY 150
#define COL_WIDTH_DOCTOR 200
#define COL_WIDTH_DATE 150
#define COL_WIDTH_HOUR 100

#define DATE_STR_LEN 20
#define HOUR_STR_LEN 16

#endif