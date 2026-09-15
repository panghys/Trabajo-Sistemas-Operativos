CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude

SRCS = src/main.cpp \
       src/config.cpp \
       src/funcionesUsuarios.cpp \
       src/funcionesPerfiles.cpp \
       src/menu.cpp

# Archivo ejecutable de salida
TARGET = main.exe

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

# NOTA: EL de arriba es el que teniamos, con ese se ejecuta todo lo de la entrega 1
# el de abajo se lo pedí a la IA para que me ayude a ejecutar lo de calculo
# si quieres ejecutar calculo, borra mientras el de arriba y usa el de abajo (lo mismo al revés)



CC = g++
CFLAGS = -Wall -std=c++11 -Icalculo/include

SRC_MULTI = calculo/mainMatrices.cpp calculo/src/matrices.cpp
EXEC_MULTI = multi

all: $(EXEC_MULTI)

$(EXEC_MULTI): $(SRC_MULTI)
	$(CC) $(CFLAGS) $(SRC_MULTI) -o $(EXEC_MULTI)

clean:
	rm -f $(EXEC_MULTI)