#ifndef CBP_H
#define CBP_H

#define NB_MAX_CLIENTS 100

bool CBP(char* requete, char* reponse, int socket);
int CBP_Login(const char* nom, const char* prenom, int numeroPatient, int nouveauPatient, int* patientId);
int CBP_Operation(char op, int a, int b);
int CBP_GetPatientsConnectes(char* buffer, int tailleBuff);
void CBP_Close();
int connecterBD();
void deconnecterBD();

#endif 