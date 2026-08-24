CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude

SRCS = bin/main.cpp \
	   src/config.cpp \
	   src/funcionesUsuarios.cpp \
	   src/funcionesPerfiles.cpp \
	   src/menu/output.cpp

TARGET = bin/main

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)