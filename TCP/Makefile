# Makefile pour la librairie TCP
CC = g++
CFLAGS = -Wall -Wextra -std=c++11 -fPIC

# Fichiers sources
SOURCES = TCP.CPP
HEADERS = TCP.h
OBJECTS = TCP.o
LIBRARY = libTCP.a

# Règle par défaut
all: $(OBJECTS) $(LIBRARY)

# Compilation des fichiers objets
$(OBJECTS): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -c $(SOURCES)

# Création de la bibliothèque statique
$(LIBRARY): $(OBJECTS)
	ar rcs $(LIBRARY) $(OBJECTS)

# Nettoyage
clean:
	rm -f $(OBJECTS) $(LIBRARY)

# Installation
install: $(LIBRARY)
	cp $(LIBRARY) ../lib/
	cp $(HEADERS) ../include/

.PHONY: all clean install
