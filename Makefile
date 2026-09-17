.PHONY: all clean pgm multi admin

all: pgm multi admin

pgm: src/main.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp src/menu.cpp calculo/palindromo.cpp calculo/fx.cpp
	g++ src/main.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp src/menu.cpp calculo/palindromo.cpp calculo/fx.cpp -o pgm

multi: calculo/mainMatrices.cpp calculo/matrices.cpp
	g++ calculo/mainMatrices.cpp calculo/matrices.cpp -o multi

admin: administrador/mainAdmin.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp
	g++ administrador/mainAdmin.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp -o admin

clean:
	rm -f pgm multi admin *.exe