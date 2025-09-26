#ifndef MAINWINDOWCLIENTCONSULTATIONBOOKER_H
#define MAINWINDOWCLIENTCONSULTATIONBOOKER_H

#include <QMainWindow>
#include <string>
#include "../TCP/TCP.h"
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
    bool connecterServeur(const string& ipServeur, int port);
    void deconnecterServeur();
    bool envoyerRequete(const string& requete, string& reponse);
    bool estConnecte() const;
    
    // Commandes CBP
    bool loginPatient(const string& nom, const string& prenom, int patientId, bool nouveauPatient);
    void logoutPatient();
    bool chargerSpecialties();
    bool chargerDocteurs();
    bool rechercherConsultations(const string& specialite, const string& docteur, 
                                const string& dateDebut, const string& dateFin);
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
    string ipServeur;
    int portServeur;
};
#endif // MAINWINDOWCLIENTCONSULTATIONBOOKER_H
