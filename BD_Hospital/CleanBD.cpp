#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>

void finish_with_error(MYSQL *con) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

int main() {
  printf("Nettoyage de la base de données...\n");
  
  MYSQL* connexion = mysql_init(NULL);
  if (!mysql_real_connect(connexion, "localhost", "Student", "PassStudent1_", "PourStudent", 0, NULL, 0)) {
    finish_with_error(connexion);
  }

  // Supprimer toutes les tables dans l'ordre inverse des dépendances
  if (mysql_query(connexion, "DROP TABLE IF EXISTS consultations;")) {
    fprintf(stderr, "Erreur lors de la suppression de la table consultations: %s\n", mysql_error(connexion));
  } else {
    printf("Table 'consultations' supprimée.\n");
  }

  if (mysql_query(connexion, "DROP TABLE IF EXISTS patients;")) {
    fprintf(stderr, "Erreur lors de la suppression de la table patients: %s\n", mysql_error(connexion));
  } else {
    printf("Table 'patients' supprimée.\n");
  }

  if (mysql_query(connexion, "DROP TABLE IF EXISTS doctors;")) {
    fprintf(stderr, "Erreur lors de la suppression de la table doctors: %s\n", mysql_error(connexion));
  } else {
    printf("Table 'doctors' supprimée.\n");
  }

  if (mysql_query(connexion, "DROP TABLE IF EXISTS specialties;")) {
    fprintf(stderr, "Erreur lors de la suppression de la table specialties: %s\n", mysql_error(connexion));
  } else {
    printf("Table 'specialties' supprimée.\n");
  }

  mysql_close(connexion);
  printf("Nettoyage de la base de données terminé.\n");
  return 0;
}