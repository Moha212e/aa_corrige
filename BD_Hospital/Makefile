# Makefile pour la création de la base de données
CC = g++
CFLAGS = -Wall -Wextra -std=c++11
MYSQL_FLAGS = -I/usr/include/mysql -m64 -L/usr/lib64/mysql -lmysqlclient -lpthread -lz -lm -lrt -lssl -lcrypto -ldl

# Fichiers sources
SOURCES = CreationBD.cpp
TARGET = CreationBD

# Règle par défaut
all: $(TARGET)

# Compilation
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(MYSQL_FLAGS)

# Nettoyage
clean:
	rm -f $(TARGET)

# Exécution
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
