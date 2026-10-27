.PHONY: all clean pgm multi admin servidor cliente

all: pgm multi admin servidor cliente

pgm: src/main.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp src/menu.cpp src/indiceInvertido.cpp calculo/palindromo.cpp calculo/fx.cpp
	g++ -std=c++17 -pthread src/main.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp src/menu.cpp src/indiceInvertido.cpp calculo/palindromo.cpp calculo/fx.cpp -o pgm

multi: calculo/mainMatrices.cpp calculo/matrices.cpp
	g++ -std=c++17 calculo/mainMatrices.cpp calculo/matrices.cpp -o multi

admin: administrador/mainAdmin.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp
	g++ -std=c++17 administrador/mainAdmin.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp -o admin

servidor: juego/mainServidor.cpp juego/servidor.cpp juego/comun.cpp src/config.cpp
	g++ -std=c++17 -Wall -pthread juego/mainServidor.cpp juego/servidor.cpp juego/comun.cpp src/config.cpp -o servidor

cliente: juego/mainCliente.cpp juego/comun.cpp src/config.cpp
	g++ -std=c++17 -Wall juego/mainCliente.cpp juego/comun.cpp src/config.cpp -o cliente

clean:
	rm -f pgm multi admin servidor cliente *.exe