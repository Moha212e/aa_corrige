CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++11 -g

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

# Include directories for headers
INCLUDES = -I$(UTIL_DIR) -I$(SOCKET_DIR) -I$(PROTOCOLE_DIR)

# Qt flags (adjust if needed)
QT_FLAGS = `pkg-config --cflags --libs Qt5Widgets`

# Default target: build everything and initialize database
default: $(BD_BIN) $(CLIENT_BIN) $(SERVEUR_BIN)
	@echo "Création et initialisation de la base de données..."

FORCE:

$(BD_BIN): FORCE $(BD_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(BD_SRC) $(MYSQL_CFLAGS) -m64 -L/usr/lib64/mysql $(MYSQL_LIBS)

$(CLIENT_BIN): FORCE $(CLIENT_SRC) $(SOCKET_HEADERS) $(UTIL_HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -fPIC -o $@ $(CLIENT_SRC) $(QT_FLAGS)

$(SERVEUR_BIN): FORCE $(SERVEUR_SRC) $(SOCKET_HEADERS) $(PROTOCOLE_HEADERS) $(UTIL_HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(SERVEUR_SRC) -lpthread $(MYSQL_CFLAGS) -m64 -L/usr/lib64/mysql $(MYSQL_LIBS)

clean:
	rm -f $(BD_BIN) $(CLIENT_BIN) $(SERVEUR_BIN) $(BD_DIR)/*.o $(CLIENT_DIR)/*.o $(SOCKET_DIR)/*.o $(SERVEUR_DIR)/*.o $(PROTOCOLE_DIR)/*.o $(UTIL_DIR)/*.o

