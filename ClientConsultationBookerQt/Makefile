# Makefile pour le client Qt
QT_DIR = /usr/lib64/qt5
QMAKE = qmake
MAKE = make

# Répertoires
TCP_DIR = ../TCP
CLIENT_DIR = .

# Fichiers sources Qt
SOURCES = main.cpp mainwindowclientconsultationbooker.cpp
HEADERS = mainwindowclientconsultationbooker.h
UI_FILES = ui_mainwindowclientconsultationbooker.h
MOC_FILES = moc_mainwindowclientconsultationbooker.cpp

# Exécutable
TARGET = ClientConsultationBooker

# Règle par défaut
all: $(TARGET)

# Compilation du client Qt
$(TARGET): $(SOURCES) $(HEADERS) $(UI_FILES) $(MOC_FILES) $(TCP_DIR)/TCP.o
	$(QMAKE) -o Makefile.qt CONFIG+=release
	$(MAKE) -f Makefile.qt
	cp ClientConsultationBooker ../bin/

# Compilation de la librairie TCP
$(TCP_DIR)/TCP.o: $(TCP_DIR)/TCP.CPP $(TCP_DIR)/TCP.h
	cd $(TCP_DIR) && $(MAKE) TCP.o

# Génération des fichiers MOC
$(MOC_FILES): $(HEADERS)
	moc mainwindowclientconsultationbooker.h -o moc_mainwindowclientconsultationbooker.cpp

# Nettoyage
clean:
	rm -f $(MOC_FILES) Makefile.qt
	rm -f *.o
	rm -f $(TARGET)
	cd $(TCP_DIR) && $(MAKE) clean

# Installation
install: $(TARGET)
	cp $(TARGET) ../bin/

# Exécution
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean install run
