
# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++11

# Directories
BD_DIR = BD_Hospital
CLIENT_DIR = ClientConsultationBookerQt
SOCKET_DIR = TCP
SERVEUR_DIR = serveur
PROTOCOLE_DIR = protocole
UTIL_DIR = util

# Source files
BD_SRC = $(BD_DIR)/CreationBD.cpp
CLIENT_SRC = $(CLIENT_DIR)/main.cpp $(CLIENT_DIR)/mainwindowclientconsultationbooker.cpp $(CLIENT_DIR)/moc_mainwindowclientconsultationbooker.cpp $(SOCKET_DIR)/TCP.CPP
SOCKET_SRC = $(SOCKET_DIR)/TCP.CPP
SERVEUR_SRC = $(SERVEUR_DIR)/serveur.cpp $(SOCKET_DIR)/TCP.CPP $(PROTOCOLE_DIR)/cbp.cpp

# Header files
SOCKET_HEADERS = $(SOCKET_DIR)/TCP.h
PROTOCOLE_HEADERS = $(PROTOCOLE_DIR)/cbp.h
UTIL_HEADERS = $(UTIL_DIR)/const.h

# Output binaries
BD_BIN = $(BD_DIR)/CreationBD
CLIENT_BIN = $(CLIENT_DIR)/ClientConsultationBooker
SERVEUR_BIN = $(SERVEUR_DIR)/serveur

# MySQL flags (headers + lib)
MYSQL_CFLAGS = -I/usr/include/mysql
MYSQL_LIBS = -lmysqlclient -lpthread -lz -lm -lrt -lssl -lcrypto -ldl

# Qt flags (adjust if needed)
QT_FLAGS = `pkg-config --cflags --libs Qt5Widgets`

# Default target
all: bin $(BD_BIN) $(CLIENT_BIN) $(SERVEUR_BIN)

# Help target
help:
	@echo "Makefile pour le projet Hospital Consultation Booker"
	@echo ""
	@echo "Cibles disponibles:"
	@echo "  all         - Compile tous les exécutables"
	@echo "  initdb      - Compile et exécute CreationBD pour initialiser la base de données"
	@echo "  setup       - Compile tout et initialise la base de données"
	@echo "  run-serveur - Lance le serveur"
	@echo "  run-client  - Lance le client Qt"
	@echo "  clean       - Supprime tous les fichiers compilés"
	@echo "  rebuild     - Clean + compile tout"
	@echo "  help        - Affiche cette aide"
	@echo ""
	@echo "Utilisation recommandée:"
	@echo "  1. make setup     (première fois)"
	@echo "  2. make run-serveur (dans un terminal)"
	@echo "  3. make run-client  (dans un autre terminal)"

# Create and initialize database
initdb: $(BD_BIN)
	@echo "Création et initialisation de la base de données..."
	./$(BD_BIN)
	@echo "Base de données initialisée avec succès!"

# Create bin directory
bin:

$(BD_BIN): $(BD_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $< $(MYSQL_CFLAGS) -m64 -L/usr/lib64/mysql $(MYSQL_LIBS)

$(CLIENT_BIN): $(CLIENT_SRC) $(SOCKET_HEADERS) $(UTIL_HEADERS)
	$(CXX) $(CXXFLAGS) -fPIC -o $@ $(CLIENT_SRC) $(QT_FLAGS)

$(SERVEUR_BIN): $(SERVEUR_SRC) $(SOCKET_HEADERS) $(PROTOCOLE_HEADERS) $(UTIL_HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(SERVEUR_SRC) -lpthread $(MYSQL_CFLAGS) -m64 -L/usr/lib64/mysql $(MYSQL_LIBS)

clean:
	rm -f $(BD_BIN) $(CLIENT_BIN) $(SERVEUR_BIN)

# Clean and rebuild everything
rebuild: clean all

# Full setup: build everything and initialize database
setup: all initdb

# Run server (after building)
run-serveur: $(SERVEUR_BIN)
	@echo "Lancement du serveur..."
	./$(SERVEUR_BIN)

# Run client (after building)
run-client: $(CLIENT_BIN)
	@echo "Lancement du client..."
	./$(CLIENT_BIN)

.PHONY: all clean bin initdb rebuild setup run-serveur run-client help