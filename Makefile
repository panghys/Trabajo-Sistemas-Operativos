all: pgm multi admin

pgm: src/main.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp src/menu.cpp src/palindromo.cpp src/fx.cpp
	g++ src/main.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp src/menu.cpp src/palindromo.cpp src/fx.cpp -o pgm

multi: src/mainMatrices.cpp src/matrices.cpp
	g++ src/mainMatrices.cpp src/matrices.cpp -o multi

admin: src/mainAdmin.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp
	g++ src/mainAdmin.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp -o admin

clean:
	rm -f pgm multi admin *.exe