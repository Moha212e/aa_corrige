# Makefile pour le protocole SMOP
CC = g++
CFLAGS = -Wall -Wextra -std=c++11 -fPIC
MYSQL_FLAGS = -I/usr/include/mysql -m64 -L/usr/lib64/mysql -lmysqlclient -lpthread -lz -lm -lrt -lssl -lcrypto -ldl

# Fichiers sources
SOURCES = smop.cpp
HEADERS = smop.h
OBJECTS = smop.o

# Règle par défaut
all: $(OBJECTS)

# Compilation des fichiers objets
$(OBJECTS): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -c $(SOURCES) $(MYSQL_FLAGS)

# Nettoyage
clean:
	rm -f $(OBJECTS)

.PHONY: all clean
