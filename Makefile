CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude -Icalculo/include

<<<<<<< Updated upstream
SRCS_MAIN = src/main.cpp \
            src/config.cpp \
            src/funcionesUsuarios.cpp \
            src/funcionesPerfiles.cpp \
            src/menu.cpp \
            calculo/src/palindromo.cpp \
            calculo/src/fx.cpp

SRCS_MULTI = calculo/mainMatrices.cpp \
             calculo/src/matrices.cpp

TARGET_MAIN = main.exe
TARGET_MULTI = multi.exe

all: $(TARGET_MAIN) $(TARGET_MULTI)

#compila el menu
$(TARGET_MAIN): $(SRCS_MAIN)
	$(CXX) $(CXXFLAGS) $(SRCS_MAIN) -o $(TARGET_MAIN)

#compila el codigo de las matrices
$(TARGET_MULTI): $(SRCS_MULTI)
	$(CXX) $(CXXFLAGS) $(SRCS_MULTI) -o $(TARGET_MULTI)
=======
pgm: src/main.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp src/menu.cpp calculo/palindromo.cpp calculo/fx.cpp
	g++ src/main.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp src/menu.cpp calculo/palindromo.cpp calculo/fx.cpp -o pgm

multi: calculo/mainMatrices.cpp calculo/matrices.cpp
	g++ calculo/mainMatrices.cpp calculo/matrices.cpp -o multi

admin: admin/mainAdmin.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp
	g++ admin/mainAdmin.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp -o admin
>>>>>>> Stashed changes

clean:
	rm -f $(TARGET_MAIN) $(TARGET_MULTI)