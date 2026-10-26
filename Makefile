.PHONY: all clean pgm multi admin

all: pgm multi admin

pgm: src/main.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp src/menu.cpp src/indiceInvertido.cpp calculo/palindromo.cpp calculo/fx.cpp
	g++ -std=c++17 -pthread src/main.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp src/menu.cpp src/indiceInvertido.cpp calculo/palindromo.cpp calculo/fx.cpp -o pgm

multi: calculo/mainMatrices.cpp calculo/matrices.cpp
	g++ -std=c++17 calculo/mainMatrices.cpp calculo/matrices.cpp -o multi

admin: administrador/mainAdmin.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp
	g++ -std=c++17 administrador/mainAdmin.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp -o admin

clean:
	rm -f pgm multi admin *.exe