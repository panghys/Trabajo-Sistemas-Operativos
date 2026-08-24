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