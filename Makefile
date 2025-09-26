
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

# Source files
BD_SRC = $(BD_DIR)/CreationBD.cpp
CLIENT_SRC = $(CLIENT_DIR)/main.cpp $(CLIENT_DIR)/mainwindowclientconsultationbooker.cpp $(CLIENT_DIR)/moc_mainwindowclientconsultationbooker.cpp $(SOCKET_DIR)/TCP.CPP
SOCKET_SRC = $(SOCKET_DIR)/TCP.CPP
SERVEUR_SRC = $(SERVEUR_DIR)/serveur.cpp $(SOCKET_DIR)/TCP.CPP $(PROTOCOLE_DIR)/smop.cpp

# Header files
SOCKET_HEADERS = $(SOCKET_DIR)/TCP.h
PROTOCOLE_HEADERS = $(PROTOCOLE_DIR)/smop.h

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

# Create bin directory
bin:
	mkdir -p bin

$(BD_BIN): $(BD_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $< $(MYSQL_CFLAGS) -m64 -L/usr/lib64/mysql $(MYSQL_LIBS)

$(CLIENT_BIN): $(CLIENT_SRC) $(SOCKET_HEADERS)
	$(CXX) $(CXXFLAGS) -fPIC -o $@ $(CLIENT_SRC) $(QT_FLAGS)

$(SERVEUR_BIN): $(SERVEUR_SRC) $(SOCKET_HEADERS) $(PROTOCOLE_HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(SERVEUR_SRC) -lpthread $(MYSQL_CFLAGS) -m64 -L/usr/lib64/mysql $(MYSQL_LIBS)

clean:
	rm -f $(BD_BIN) $(CLIENT_BIN) $(SERVEUR_BIN)
	rm -rf bin

.PHONY: all clean bin