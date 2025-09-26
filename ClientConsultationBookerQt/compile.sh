#!/bin/bash
# Script de compilation pour le client Qt

echo "🔨 Compilation du client Qt..."

# Vérifier que Qt est installé
if ! command -v qmake &> /dev/null; then
    echo "❌ Erreur: qmake n'est pas installé"
    echo "Installez Qt5 avec: sudo yum install qt5-qtbase-devel"
    exit 1
fi

# Compiler la librairie TCP d'abord
echo "📦 Compilation de la librairie TCP..."
cd ../TCP
make clean
make
if [ $? -ne 0 ]; then
    echo "❌ Erreur lors de la compilation de TCP"
    exit 1
fi
cd ../ClientConsultationBookerQt

# Générer les fichiers MOC
echo "🔄 Génération des fichiers MOC..."
moc mainwindowclientconsultationbooker.h -o moc_mainwindowclientconsultationbooker.cpp

# Compiler le client Qt
echo "🔨 Compilation du client Qt..."
g++ -std=c++11 -fPIC -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtCore -I/usr/include/qt5/QtGui -I../TCP \
    main.cpp mainwindowclientconsultationbooker.cpp moc_mainwindowclientconsultationbooker.cpp \
    ../TCP/TCP.o \
    -lQt5Widgets -lQt5Core -lQt5Gui -lpthread \
    -o ClientConsultationBooker

if [ $? -eq 0 ]; then
    echo "✅ Compilation du client Qt réussie !"
    echo "Exécutable créé: ClientConsultationBooker"
else
    echo "❌ Erreur lors de la compilation du client Qt"
    exit 1
fi
