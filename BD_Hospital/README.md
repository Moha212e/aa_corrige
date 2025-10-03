# BD_Hospital - Création de Base de Données

## Description
Programme utilitaire pour créer et initialiser la base de données MySQL du système de réservation hospitalier.

## Objectif
Automatiser la création des tables et l'insertion des données de test pour le fonctionnement du système de réservation.

## Fichiers
- **`CreationBD.cpp`** : Programme principal de création de la BD
- **`CreationBD`** : Exécutable compilé (après `make`)

## Base de données créée
- **Nom** : `PourStudent`
- **Utilisateur** : `Student` 
- **Mot de passe** : `PassStudent1_`
- **Serveur** : `localhost`

## Tables créées

### 📋 specialties
```sql
CREATE TABLE specialties (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(30)
);
```
**Données** : Cardiologie, Dermatologie, Généraliste, Pédiatrie, Psychiatrie

### 👨‍⚕️ doctors  
```sql
CREATE TABLE doctors (
    id INT AUTO_INCREMENT PRIMARY KEY,
    specialty_id INT,
    last_name VARCHAR(30),
    first_name VARCHAR(30),
    FOREIGN KEY (specialty_id) REFERENCES specialties(id)
);
```
**Données** : 6 médecins répartis dans les spécialités

### 👥 patients
```sql  
CREATE TABLE patients (
    id INT AUTO_INCREMENT PRIMARY KEY,
    last_name VARCHAR(30),
    first_name VARCHAR(30),
    birth_date VARCHAR(20)
);
```
**Données** : 2 patients de test (Durand Jean, Petit Sophie)

### 📅 consultations
```sql
CREATE TABLE consultations (
    id INT AUTO_INCREMENT PRIMARY KEY,
    doctor_id INT NOT NULL,
    patient_id INT DEFAULT NULL,
    date VARCHAR(20),
    hour VARCHAR(10),
    reason VARCHAR(100),
    FOREIGN KEY (doctor_id) REFERENCES doctors(id),
    FOREIGN KEY (patient_id) REFERENCES patients(id)
);
```
**Données** : 11 consultations (3 déjà réservées, 8 disponibles)

## Données de test incluses

### Spécialités pré-remplies
1. Cardiologie
2. Dermatologie  
3. Généraliste
4. Pédiatrie
5. Psychiatrie

### Médecins pré-remplis
- Dr. Dubois Pierre (Cardiologie)
- Dr. Martin Claire (Dermatologie)
- Dr. Durand Michel (Généraliste)
- Dr. Leroy Anne (Pédiatrie)
- Dr. Moreau Jean (Psychiatrie)
- Dr. Simon Marie (Généraliste)

### Consultations disponibles
- **Période** : Octobre 2025
- **Horaires** : 9h00 à 14h30
- **État** : Certaines déjà réservées, d'autres disponibles

## Utilisation

### Compilation
```bash
make
# Ou directement :
g++ -o BD_Hospital/CreationBD BD_Hospital/CreationBD.cpp -lmysqlclient
```

### Exécution
```bash
./BD_Hospital/CreationBD
```

### Prérequis MySQL
```sql
-- Créer l'utilisateur (si nécessaire)
CREATE USER 'Student'@'localhost' IDENTIFIED BY 'PassStudent1_';
GRANT ALL PRIVILEGES ON PourStudent.* TO 'Student'@'localhost';
FLUSH PRIVILEGES;
```

## Comportement du programme
1. **Connexion** à MySQL avec les identifiants Student
2. **Suppression** des tables existantes (DROP TABLE IF EXISTS)
3. **Création** des tables dans l'ordre (spécialités → médecins → patients → consultations)
4. **Insertion** des données de test
5. **Fermeture** propre de la connexion

## Gestion d'erreurs
- Vérification de la connexion MySQL
- Validation de chaque requête SQL
- Messages d'erreur détaillés
- Arrêt propre en cas de problème

## Structure des données
Le programme utilise des structures C pour organiser les données :
```c
typedef struct {
    int id;
    char name[30];
} SPECIALTY;

typedef struct {
    int id;
    int specialty_id;
    char last_name[30];
    char first_name[30];
} DOCTOR;
// etc.
```

## Dépendances
- MySQL C API (`mysql.h`)
- Serveur MySQL configuré et démarré
- Permissions appropriées pour l'utilisateur Student