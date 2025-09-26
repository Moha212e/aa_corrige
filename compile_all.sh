#!/bin/bash
# Script de compilation global pour tout le projet

echo "🚀 Compilation complète du projet de réservation de consultations"
echo "================================================================"

# Créer le répertoire bin
mkdir -p bin

# 1. Compiler la base de données
echo ""
echo "1️⃣ Compilation de la base de données..."
cd BD_Hospital
make clean
make
if [ $? -eq 0 ]; then
    cp CreationBD ../bin/
    echo "✅ Base de données compilée"
else
    echo "❌ Erreur compilation base de données"
    exit 1
fi
cd ..

# 2. Compiler la librairie TCP
echo ""
echo "2️⃣ Compilation de la librairie TCP..."
cd TCP
make clean
make
if [ $? -eq 0 ]; then
    echo "✅ Librairie TCP compilée"
else
    echo "❌ Erreur compilation TCP"
    exit 1
fi
cd ..

# 3. Compiler le protocole
echo ""
echo "3️⃣ Compilation du protocole..."
cd protocole
make clean
make
if [ $? -eq 0 ]; then
    echo "✅ Protocole compilé"
else
    echo "❌ Erreur compilation protocole"
    exit 1
fi
cd ..

# 4. Compiler le serveur
echo ""
echo "4️⃣ Compilation du serveur..."
cd serveur
make clean
make
if [ $? -eq 0 ]; then
    cp serveur ../bin/
    cp serveur.conf ../bin/
    echo "✅ Serveur compilé"
else
    echo "❌ Erreur compilation serveur"
    exit 1
fi
cd ..

# 5. Compiler le client Qt
echo ""
echo "5️⃣ Compilation du client Qt..."
cd ClientConsultationBookerQt
chmod +x compile.sh
./compile.sh
if [ $? -eq 0 ]; then
    cp ClientConsultationBooker ../bin/
    echo "✅ Client Qt compilé"
else
    echo "❌ Erreur compilation client Qt"
    exit 1
fi
cd ..

echo ""
echo "🎉 COMPILATION TERMINÉE AVEC SUCCÈS !"
echo "======================================"
echo ""
echo "📁 Exécutables créés dans le répertoire 'bin/':"
echo "   - CreationBD (création base de données)"
echo "   - serveur (serveur de réservation)"
echo "   - ClientConsultationBooker (client Qt)"
echo ""
echo "🚀 Pour démarrer le projet :"
echo "   1. Créer la BD : ./bin/CreationBD"
echo "   2. Lancer le serveur : ./bin/serveur serveur/serveur.conf"
echo "   3. Lancer le client : ./bin/ClientConsultationBooker"
echo ""
