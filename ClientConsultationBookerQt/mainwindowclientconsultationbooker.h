#ifndef MAINWINDOWCLIENTCONSULTATIONBOOKER_H
#define MAINWINDOWCLIENTCONSULTATIONBOOKER_H

#include <QMainWindow>
#include <string>
#include "../TCP/TCP.h"
#include "../util/const.h"
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowClientConsultationBooker; }
QT_END_NAMESPACE

class MainWindowClientConsultationBooker : public QMainWindow
{
    Q_OBJECT

public:
    MainWindowClientConsultationBooker(QWidget *parent = nullptr);
    ~MainWindowClientConsultationBooker();

    void addTupleTableConsultations(int id, string specialty, string doctor, string date, string hour);
    void clearTableConsultations();
    int getSelectionIndexTableConsultations() const;

    void addComboBoxSpecialties(string specialty);
    string getSelectionSpecialty() const;
    void clearComboBoxSpecialties();

    void addComboBoxDoctors(string doctor);
    string getSelectionDoctor() const;
    void clearComboBoxDoctors();

    string getLastName() const;
    string getFirstName() const;
    int getPatientId() const;
    bool isNewPatientSelected() const;
    string getStartDate() const;
    string getEndDate() const;
    void setLastName(string value);
    void setFirstName(string value);
    void setPatientId(int value);
    void setNewPatientChecked(bool state);
    void setStartDate(string date);
    void setEndDate(string date);

    void loginOk();
    void logoutOk();

    void dialogMessage(const string& title,const string& message);
    void dialogError(const string& title,const string& message);
    string dialogInputText(const string& title,const string& question);
    int dialogInputInt(const string& title,const string& question);

    // Fonctions de communication réseau
    /**
     * Établit une connexion TCP avec le serveur de réservation
     * 
     * @param ipServeur: Adresse IP du serveur à contacter
     * @param port: Numéro de port du serveur
     * 
     * @return bool: 
     *   - true: Connexion établie avec succès
     *   - false: Erreur lors de la connexion
     * 
     * Cette fonction sert à:
     * - Créer un socket client TCP
     * - Se connecter au serveur de réservation
     * - Initialiser la communication réseau
     * - Préparer l'envoi de requêtes
     * 
     * Note: Met à jour l'état de connexion interne
     */
    bool connecterServeur(const string& ipServeur, int port);
    
    /**
     * Ferme la connexion avec le serveur
     * 
     * @return void: Aucune valeur de retour
     * 
     * Cette fonction sert à:
     * - Fermer proprement le socket TCP
     * - Libérer les ressources réseau
     * - Mettre à jour l'état de connexion
     * - Assurer un arrêt propre
     * 
     * Note: Appelée lors de la déconnexion ou fermeture de l'application
     */
    void deconnecterServeur();
    
    /**
     * Envoie une requête au serveur et récupère la réponse
     * 
     * @param requete: Chaîne de caractères contenant la requête à envoyer
     * @param reponse: Buffer de sortie pour stocker la réponse du serveur
     * 
     * @return bool: 
     *   - true: Communication réussie
     *   - false: Erreur lors de la communication
     * 
     * Cette fonction sert à:
     * - Transmettre les requêtes au serveur
     * - Recevoir les réponses du serveur
     * - Gérer les erreurs de communication
     * - Maintenir la synchronisation client-serveur
     * 
     * Note: Fonction bloquante jusqu'à réception de la réponse
     */
    bool envoyerRequete(const string& requete, string& reponse);
    
    /**
     * Vérifie l'état de la connexion avec le serveur
     * 
     * @return bool: 
     *   - true: Client connecté au serveur
     *   - false: Client déconnecté
     * 
     * Cette fonction sert à:
     * - Vérifier l'état de la connexion réseau
     * - Valider la disponibilité du serveur
     * - Contrôler l'authentification
     * - Gérer l'état de l'interface utilisateur
     * 
     * Note: Utilisée pour activer/désactiver les fonctionnalités
     */
    bool estConnecte() const;
    
    // Commandes CBP
    /**
     * Authentifie un patient sur le serveur de réservation
     * 
     * @param nom: Nom de famille du patient
     * @param prenom: Prénom du patient
     * @param patientId: Identifiant unique du patient
     * @param nouveauPatient: Indique si c'est un nouveau patient
     * 
     * @return bool: 
     *   - true: Authentification réussie
     *   - false: Échec de l'authentification
     * 
     * Cette fonction sert à:
     * - Connecter le patient au système
     * - Valider les identifiants du patient
     * - Initialiser la session utilisateur
     * - Activer les fonctionnalités de réservation
     * 
     * Note: Utilise le protocole CBP pour l'authentification
     */
    bool loginPatient(const string& nom, const string& prenom, int patientId, bool nouveauPatient);
    
    /**
     * Déconnecte le patient du serveur
     * 
     * @return void: Aucune valeur de retour
     * 
     * Cette fonction sert à:
     * - Fermer la session utilisateur
     * - Déconnecter le patient du système
     * - Libérer les ressources de session
     * - Désactiver les fonctionnalités
     * 
     * Note: Appelée lors de la déconnexion ou fermeture de l'application
     */
    void logoutPatient();
    
    /**
     * Charge la liste des spécialités médicales depuis le serveur
     * 
     * @return bool: 
     *   - true: Chargement réussi
     *   - false: Erreur lors du chargement
     * 
     * Cette fonction sert à:
     * - Récupérer les spécialités disponibles
     * - Peupler la liste déroulante des spécialités
     * - Permettre la sélection de spécialités
     * - Faciliter la recherche de consultations
     * 
     * Note: Met à jour l'interface utilisateur avec les spécialités
     */
    bool chargerSpecialties();
    
    /**
     * Charge la liste des médecins depuis le serveur
     * 
     * @return bool: 
     *   - true: Chargement réussi
     *   - false: Erreur lors du chargement
     * 
     * Cette fonction sert à:
     * - Récupérer les médecins disponibles
     * - Peupler la liste déroulante des médecins
     * - Permettre la sélection de médecins
     * - Faciliter la recherche de consultations
     * 
     * Note: Met à jour l'interface utilisateur avec les médecins
     */
    bool chargerDocteurs();
    
    /**
     * Recherche des consultations selon les critères spécifiés
     * 
     * @param specialite: Spécialité médicale recherchée
     * @param docteur: Nom du médecin recherché
     * @param dateDebut: Date de début de recherche
     * @param dateFin: Date de fin de recherche
     * 
     * @return bool: 
     *   - true: Recherche réussie
     *   - false: Erreur lors de la recherche
     * 
     * Cette fonction sert à:
     * - Filtrer les consultations selon les critères
     * - Afficher les résultats dans le tableau
     * - Permettre la sélection de consultations
     * - Faciliter la réservation
     * 
     * Note: Met à jour le tableau des consultations disponibles
     */
    bool rechercherConsultations(const string& specialite, const string& docteur, 
                                const string& dateDebut, const string& dateFin);
    
    /**
     * Réserve une consultation pour le patient connecté
     * 
     * @param consultationId: Identifiant de la consultation à réserver
     * @param raison: Raison de la consultation
     * 
     * @return bool: 
     *   - true: Réservation réussie
     *   - false: Erreur lors de la réservation
     * 
     * Cette fonction sert à:
     * - Réserver la consultation sélectionnée
     * - Associer le patient à la consultation
     * - Enregistrer la raison de la consultation
     * - Confirmer la réservation
     * 
     * Note: Utilise le protocole CBP pour la réservation
     */
    bool reserverConsultation(int consultationId, const string& raison);


private slots:
    void on_pushButtonLogin_clicked();
    void on_pushButtonLogout_clicked();
    void on_pushButtonRechercher_clicked();
    void on_pushButtonReserver_clicked();

private:
    Ui::MainWindowClientConsultationBooker *ui;
    
    // Variables de communication réseau
    int socketServeur;
    bool connecte;
    string ipServeur = "127.0.0.1";
    int portServeur = 8080;

protected:
    void closeEvent(QCloseEvent* event) override;
};
#endif // MAINWINDOWCLIENTCONSULTATIONBOOKER_H
