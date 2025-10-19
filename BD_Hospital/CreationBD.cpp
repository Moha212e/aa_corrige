#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <time.h>
#include <string.h>

typedef struct {
  int  id;
  char name[30];
} SPECIALTY;

typedef struct {
  int  id;
  int  specialty_id;
  char last_name[30];
  char first_name[30];
} DOCTOR;

typedef struct {
  int  id;
  char last_name[30];
  char first_name[30];
  char birth_date[20];
} PATIENT;

typedef struct {
  int  id;
  int  doctor_id;
  int  patient_id;
  char date[20];
  char hour[10];
  char reason[100];
} CONSULTATION;

SPECIALTY specialties[] = {
  {-1, "Cardiologie"},
  {-1, "Dermatologie"},
  {-1, "Neurologie"},
  {-1, "Ophtalmologie"}
};
int nbSpecialties = 4;

DOCTOR doctors[] = {
  {-1, 1, "Dupont", "Alice"},
  {-1, 2, "Lemoine", "Bernard"},
  {-1, 3, "Martin", "Claire"},
  {-1, 2, "Maboul", "Paul"},
  {-1, 1, "Coptere", "Elie"},
  {-1, 3, "Merad", "Gad"}
};
int nbDoctors = 6;

PATIENT patients[] = {
  {-1, "Durand", "Jean", "1980-05-12"},
  {-1, "Petit", "Sophie", "1992-11-30"}
};
int nbPatients = 2;

CONSULTATION consultations[] = {
  // Consultations déjà réservées
  {-1, 1,  1, "2025-10-01", "09:00", "Check-up cardiologique"},
  {-1, 2,  2, "2025-10-02", "14:30", "Premier rendez-vous dermatologique"},
  {-1, 3,  1, "2025-10-03", "11:15", "Consultation neurologique"},
  
  // Consultations libres - Cardiologie (Dr. Dupont, Dr. Coptere)
  {-1, 1, -1, "2025-10-15", "09:00", ""},
  {-1, 1, -1, "2025-10-15", "09:30", ""},
  {-1, 1, -1, "2025-10-15", "10:00", ""},
  {-1, 1, -1, "2025-10-15", "10:30", ""},
  {-1, 5, -1, "2025-10-16", "09:00", ""},
  {-1, 5, -1, "2025-10-16", "09:30", ""},
  {-1, 5, -1, "2025-10-16", "10:00", ""},
  
  // Consultations libres - Dermatologie (Dr. Lemoine, Dr. Maboul)
  {-1, 2, -1, "2025-10-17", "14:00", ""},
  {-1, 2, -1, "2025-10-17", "14:30", ""},
  {-1, 2, -1, "2025-10-17", "15:00", ""},
  {-1, 4, -1, "2025-10-18", "14:00", ""},
  {-1, 4, -1, "2025-10-18", "14:30", ""},
  {-1, 4, -1, "2025-10-18", "15:00", ""},
  
  // Consultations libres - Neurologie (Dr. Martin, Dr. Merad)
  {-1, 3, -1, "2025-10-19", "10:00", ""},
  {-1, 3, -1, "2025-10-19", "10:30", ""},
  {-1, 3, -1, "2025-10-19", "11:00", ""},
  {-1, 6, -1, "2025-10-20", "10:00", ""},
  {-1, 6, -1, "2025-10-20", "10:30", ""},
  {-1, 6, -1, "2025-10-20", "11:00", ""},
  
  // Consultations libres - Ophtalmologie (nouveaux créneaux)
  {-1, 1, -1, "2025-10-21", "08:00", ""},
  {-1, 1, -1, "2025-10-21", "08:30", ""},
  {-1, 2, -1, "2025-10-22", "16:00", ""},
  {-1, 2, -1, "2025-10-22", "16:30", ""},
  {-1, 3, -1, "2025-10-23", "09:00", ""},
  {-1, 3, -1, "2025-10-23", "09:30", ""},
  {-1, 4, -1, "2025-10-24", "11:00", ""},
  {-1, 4, -1, "2025-10-24", "11:30", ""},
  {-1, 5, -1, "2025-10-25", "13:00", ""},
  {-1, 5, -1, "2025-10-25", "13:30", ""},
  {-1, 6, -1, "2025-10-26", "15:00", ""},
  {-1, 6, -1, "2025-10-26", "15:30", ""},
};
int nbConsultations = 36;

void finish_with_error(MYSQL *con) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

int main() {
  MYSQL* connexion = mysql_init(NULL);
  if (!mysql_real_connect(connexion, "localhost", "Student", "PassStudent1_", "PourStudent", 0, NULL, 0)) {
    finish_with_error(connexion);
  }

  // Création de la base de données si elle n'existe pas
  mysql_query(connexion, "CREATE DATABASE IF NOT EXISTS PourStudent CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;");
  mysql_query(connexion, "USE PourStudent;");

  // Suppression des anciennes tables
  mysql_query(connexion, "DROP TABLE IF EXISTS consultations;");
  mysql_query(connexion, "DROP TABLE IF EXISTS patient;");
  mysql_query(connexion, "DROP TABLE IF EXISTS doctor;");
  mysql_query(connexion, "DROP TABLE IF EXISTS specialties;");

  // Création de la table specialties
  if (mysql_query(connexion, "CREATE TABLE IF NOT EXISTS specialties ("
                             "id INT AUTO_INCREMENT PRIMARY KEY, "
                             "name VARCHAR(100) NOT NULL UNIQUE COMMENT 'Nom de la spécialité médicale', "
                             "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT 'Date de création', "
                             "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT 'Date de dernière modification'"
                             ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci "
                             "COMMENT='Table des spécialités médicales';"))
    finish_with_error(connexion);

  // Création de la table patient
  if (mysql_query(connexion, "CREATE TABLE IF NOT EXISTS patient ("
                             "id INT AUTO_INCREMENT PRIMARY KEY, "
                             "first_name VARCHAR(100) NOT NULL COMMENT 'Prénom du patient', "
                             "last_name VARCHAR(100) NOT NULL COMMENT 'Nom de famille du patient', "
                             "birth_date DATE COMMENT 'Date de naissance du patient', "
                             "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT 'Date de création', "
                             "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT 'Date de dernière modification', "
                             "INDEX idx_last_name (last_name), "
                             "INDEX idx_birth_date (birth_date), "
                             "INDEX idx_full_name (first_name, last_name)"
                             ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci "
                             "COMMENT='Table des patients';"))
    finish_with_error(connexion);

  // Création de la table doctor
  if (mysql_query(connexion, "CREATE TABLE IF NOT EXISTS doctor ("
                             "id INT AUTO_INCREMENT PRIMARY KEY, "
                             "first_name VARCHAR(100) NOT NULL COMMENT 'Prénom du médecin', "
                             "last_name VARCHAR(100) NOT NULL COMMENT 'Nom de famille du médecin', "
                             "password VARCHAR(255) NOT NULL COMMENT 'Mot de passe du médecin (hashé)', "
                             "specialite_id INT COMMENT 'ID de la spécialité du médecin', "
                             "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT 'Date de création', "
                             "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT 'Date de dernière modification', "
                             "FOREIGN KEY (specialite_id) REFERENCES specialties(id) ON DELETE SET NULL, "
                             "INDEX idx_last_name (last_name), "
                             "INDEX idx_specialite (specialite_id), "
                             "INDEX idx_full_name (first_name, last_name)"
                             ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci "
                             "COMMENT='Table des médecins';"))
    finish_with_error(connexion);

  // Création de la table consultations
  if (mysql_query(connexion, "CREATE TABLE IF NOT EXISTS consultations ("
                             "id INT AUTO_INCREMENT PRIMARY KEY, "
                             "patient_id INT NULL COMMENT 'ID du patient (NULL = consultation libre)', "
                             "doctor_id INT NOT NULL COMMENT 'ID du médecin', "
                             "date DATE NOT NULL COMMENT 'Date de la consultation', "
                             "hour TIME COMMENT 'Heure de la consultation', "
                             "reason TEXT COMMENT 'Motif de la consultation', "
                             "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT 'Date de création', "
                             "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT 'Date de dernière modification', "
                             "FOREIGN KEY (patient_id) REFERENCES patient(id) ON DELETE CASCADE, "
                             "FOREIGN KEY (doctor_id) REFERENCES doctor(id) ON DELETE CASCADE, "
                             "INDEX idx_date (date), "
                             "INDEX idx_patient (patient_id), "
                             "INDEX idx_doctor (doctor_id), "
                             "INDEX idx_doctor_date (doctor_id, date), "
                             "INDEX idx_patient_date (patient_id, date), "
                             "UNIQUE KEY unique_appointment (doctor_id, date, hour)"
                             ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci "
                             "COMMENT='Table des consultations médicales';"))
    finish_with_error(connexion);

  char request[512];
  
  // Insertion des spécialités
  for (int i = 0; i < nbSpecialties; i++) {
    sprintf(request, "INSERT INTO specialties (name) VALUES ('%s');", specialties[i].name);
    if (mysql_query(connexion, request)) finish_with_error(connexion);
  }

  // Insertion des médecins (avec mot de passe par défaut 'test')
  for (int i = 0; i < nbDoctors; i++) {
    sprintf(request, "INSERT INTO doctor (first_name, last_name, password, specialite_id) VALUES ('%s', '%s', 'test', %d);",
            doctors[i].first_name, doctors[i].last_name, doctors[i].specialty_id);
    if (mysql_query(connexion, request)) finish_with_error(connexion);
  }

  // Insertion des patients
  for (int i = 0; i < nbPatients; i++) {
    sprintf(request, "INSERT INTO patient (first_name, last_name, birth_date) VALUES ('%s', '%s', '%s');",
            patients[i].first_name, patients[i].last_name, patients[i].birth_date);
    if (mysql_query(connexion, request)) finish_with_error(connexion);
  }

  // Insertion des consultations
  for (int i = 0; i < nbConsultations; i++) {
    if (consultations[i].patient_id == -1) {
      sprintf(request, "INSERT INTO consultations (doctor_id, patient_id, date, hour, reason) "
                       "VALUES (%d, NULL, '%s', '%s', '%s');",
              consultations[i].doctor_id, consultations[i].date, consultations[i].hour, consultations[i].reason);
    } else {
      sprintf(request, "INSERT INTO consultations (doctor_id, patient_id, date, hour, reason) "
                       "VALUES (%d, %d, '%s', '%s', '%s');",
              consultations[i].doctor_id, consultations[i].patient_id, consultations[i].date,
              consultations[i].hour, consultations[i].reason);
    }
    printf("DEBUG: Requête consultation %d = %s\n", i+1, request);
    if (mysql_query(connexion, request)) {
      printf("Erreur sur la consultation %d\n", i+1);
      finish_with_error(connexion);
    }
  }

  mysql_close(connexion);
  return 0;
}

