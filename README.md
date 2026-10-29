# SistOpe - Administrador de Usuarios y Perfiles

## 1. Propósito de la aplicación

Esta aplicación presenta al usuario un menú con varias opciones, a las que se accede según el input entregado y considerando los permisos del usuario (admin o usuario general).

Además del menú, el sistema incluye un creador concurrente de índice invertido y un juego multijugador por turnos que funciona mediante sockets.

El proyecto incluye la carpeta `LIBROS`, que contiene 50 MB de archivos de texto separados por género.

## 2. Cómo se debe ejecutar

El proyecto está desarrollado en C++17 y utiliza `make` para la compilación. Las funcionalidades de la Entrega 3 (threads, semáforos y sockets) usan APIs de Linux, por lo que el sistema debe compilarse y ejecutarse en Linux

### Paso 1: Compilar el código

Abrir una terminal en la raíz del proyecto y ejecutar:

```
make
```

Esto genera cinco ejecutables: `pgm` (menú principal), `multi` (multiplicador de matrices), `admin` (administración de usuarios y perfiles), `servidor` y `cliente` (juego). Para empezar de cero se usa `make clean` y luego `make`.

### Paso 2: Ejecutar el menú principal

```
./pgm -u [usuario] -p [contraseña] -f [algún archivo]
```

Donde `[usuario]` es el nombre de usuario, `[contraseña]` la clave del usuario y `[algún archivo]` la ruta de un archivo para ejecutar una de las funciones del programa, por ejemplo `data/MatrizA.txt`.

Si el sistema está vacío, se genera automáticamente un usuario administrador. Después de eso se puede ejecutar con:

```
./pgm -u admin -p 1234 -f data/MatrizA.txt
```

Todos los programas deben ejecutarse desde la raíz del proyecto, porque leen el archivo `.env` de esa carpeta.

### Paso 3: Crear el índice invertido

En el menú principal elegir la opción `8. Crear indice invertido`.

### Paso 4: Jugar

El juego se ejecuta con dos programas distintos. Primero se inicia el servidor y luego se conecta cada jugador, cada uno en una terminal propia:

```
./servidor
./cliente
```

## 3. Variables de entorno

Todas las variables se leen desde el archivo `.env` de la raíz del proyecto. Antes de usarlas, el programa valida que existan y que su valor sea válido; si no lo son, muestra un mensaje de error y no continúa.

### Sistema, libros e índice invertido

| Variable | Descripción | Valor de ejemplo |
| :--- | :--- | :--- |
| `USER_FILE` | Ruta del archivo que almacena los registros de los usuarios. | `data/USUARIOS.TXT` |
| `PERFIL_FILE` | Ruta del archivo que almacena los perfiles y sus opciones de menú permitidas. | `data/PERFILES.TXT` |
| `LIBROS_DIR` | Ruta de la carpeta que contiene los libros (archivos `.txt`, se buscan también en subcarpetas). | `LIBROS` |
| `INDICE_FILE` | Ruta del archivo donde se guarda el índice invertido (`indice_invertido.idx`). | `data/indice_invertido.idx` |
| `MAPA_LIBROS_FILE` | Ruta del archivo que enumera los libros, asociando cada uno con un ID. Ese ID es el que se usa en el índice invertido. | `data/mapa_libros.txt` |
| `N_THREADS` | Cantidad de threads trabajadores que construyen el índice invertido (entero mayor a 0). | `4` |

### Juego

| Variable | Usada por | Descripción | Valor de ejemplo |
| :--- | :--- | :--- | :--- |
| `GAME_HOST` | cliente | Dirección IP o nombre del equipo donde corre el servidor. | `127.0.0.1` |
| `GAME_PORT` | servidor y cliente | Puerto TCP para la comunicación entre servidor y jugadores (1024 a 65535). | `5050` |
| `GAME_LOG_FILE` | servidor | Ruta del archivo donde se guarda el registro (log) de eventos del juego. | `data/juego.log` |
| `GAME_BOARD_SIZE` | servidor | Cantidad de posiciones del tablero (10 a 1000). | `50` |
| `GAME_DICE_FACES` | servidor | Número de caras del dado (2 a 100). | `6` |

## 4. Funcionalidades

- **Administración de usuarios y perfiles:** el acceso al programa requiere un usuario, y cada usuario tiene un perfil que permite bloquear ciertas opciones según su nivel de acceso (admin o usuario). El sistema permite crear nuevos perfiles y gestionar su nivel de acceso.
- **Módulo de cálculo:** permite ejecutar una función f(x) específica y operaciones con matrices leídas desde archivos. También ofrece funciones sobre texto: saber si una palabra es palíndromo y contar vocales, consonantes, caracteres especiales y palabras de un texto o archivo.
- **Creación del índice invertido (opción 8):** construye en paralelo el índice invertido de todos los libros de la carpeta `LIBROS`.
- **Juego multijugador por turnos:** juego de tablero por equipos que funciona con una arquitectura cliente-servidor mediante sockets.

## 5. Índice invertido: funcionamiento técnico

Para la creacion del indice se capturaa cada texto plano (.txt) del directorio de libros se le asigna un id y se guarda en una colaa de trabajo. se procede a ejecutar un proceso en paralelo con una cantidad definida N_THREADS de hilos de trabajo, cada hilo solicita un elemento de laa colaa, esperando la señal de un semaforo que indica que laa cola este desupada paara evitar coliciones, una vez con su libro lo procesa y mezcla su resultado particulaaar a otra estrucutra que posee todos los resultados a nivel global, haci hasta que se aagote la cola. cabe recalcar que existen tres semaforos: uno para la cola, otro para la estructura qie posee los resultados globaales y una ultima para laa consola de salida.

## 6. Juego multijugador: funcionamiento técnico

### Arquitectura

**clientes -> sockets -> servidor -> estado compartido del juego**

El servidor empieza a escuchar conexiones TCP en el port (GAME_PORT). Cada cliente que se conecta se le crea un thread que escucha los comandos
Todos los threads comparten el estado del juego (osea, los equipos, posiciones, turnos,etc) y este esta protegido por un semáforo. Este semáforo se encarga de vigilar que dos threads no ejecuten
comandos a la vez. En caso de que exista algo similar, los deja pendientes. Si se pueden ejecutar luego de que el primero pudo, por ejemplo, dos usuarios mandan un comando, se toma el del primero válido (que le tocaba)
y al segundo luego no le tocaba, se cancela.
El cliente luego se conecta al host, se le pide nombre y equipo y luego queda pendiente del teclado y los mensajes del server. El servidor responde a los comandos.
Todo depende del server en cuanto a validación: las acciones, el sorteo del dado, los estados y los avisos a jugadores

### Cómo ejecutar el juego

1. En una terminal, iniciar el servidor: `./servidor`
2. En otras terminales, hay que ejecutar `./cliente` e ingresar un nombre y la letra del equipo
3. Con 2 equipos de 2 jugadores, la partida comienza sola. Cuando sea el turno del jugador, aparece el mensaje `>>> Es tu turno` y debe escribir `lanzar`.
4. Para jugar desde otros computadores, cambiar `GAME_HOST` por la IP del equipo donde corre el servidor.

Comandos del cliente: `lanzar`, `chat <texto>`, `estado`, `ayuda` y `salir`.

### Reglas del juego

- **Jugadores:** mínimo 4 y máximo 16 jugadores simultáneos. Cada jugador tiene un nombre único (letras, números o `_`, hasta 20 caracteres).
- **Equipos:** entre 2 y 4 equipos (`EquipoA` a `EquipoD`), con 2 a 4 jugadores por equipo. Cada jugador elige su equipo al conectarse.
- **Condición de inicio:** la partida comienza automáticamente cuando existen al menos 2 equipos y cada equipo tiene al menos 2 jugadores. Mientras no se cumpla, el servidor informa a todos los jugadores qué falta. Una vez iniciada, no se aceptan nuevos jugadores.
- **Tablero:** tiene `GAME_BOARD_SIZE` posiciones. Todos los equipos parten en la posición 0 y avanzan como equipo.
- **Turnos:** los equipos juegan en orden (A, B, C, D) y, dentro de cada equipo, los jugadores se rotan. El servidor controla los turnos: si un jugador intenta actuar fuera de su turno, la acción se rechaza con un mensaje de error y queda registrada en el log.
- **Acciones permitidas:**
  - `lanzar`: solo en el turno propio. Equivale a lanzar un dado de `GAME_DICE_FACES` caras (resultado aleatorio entre 1 y R); el resultado se suma a la posición del equipo.
  - `chat <texto>`: envía un mensaje a todos los jugadores, en cualquier momento.
  - `estado`: muestra las posiciones de los equipos y de quién es el turno.
  - `salir`: abandona la partida.
- **Condición de término:** la partida termina cuando un equipo alcanza o supera la posición `GAME_BOARD_SIZE`, y ese equipo gana. También termina si todos los equipos menos uno abandonan (gana el que queda) o si el servidor se detiene con `Ctrl+C`. Al terminar, el servidor informa el resultado a todos los jugadores.
- **Desconexiones:** si un jugador se desconecta, el servidor sigue funcionando. Si era su turno, este pasa al siguiente jugador conectado, y los jugadores desconectados se saltan en los turnos siguientes.
- **Cierre del servidor:** al terminar la partida, el servidor cierra los sockets, espera a sus threads, cierra el archivo de log y libera los semáforos.

### Archivo de log

El servidor escribe en `GAME_LOG_FILE` una línea por evento, con el formato:

```
fecha hora;jugador;equipo;ACCION;detalle
```
Eventos registrados: `INICIA_SERVIDOR`, `CONECTA`, `UNE_EQUIPO`, `INICIA_PARTIDA`, `INICIA_TURNO`, `CAMBIA_TURNO`, `LANZA_DADO`, `AVANZA`, `CHAT`, `ACCION_RECHAZADA`, `RECHAZA_CONEXION`, `GANA`, `TERMINA_PARTIDA`, `RESULTADO_FINAL`, `DESCONECTA`, `CONEXION_CERRADA` y `CIERRA_SERVIDOR`.

## 7. Compilación y ejecución manual

Si no se usa `make`, cada programa se puede compilar a mano desde la raíz del proyecto:

| Programa | Comando de compilación | Ejecución |
| :--- | :--- | :--- |
| **Menú principal** | `g++ -std=c++17 -pthread src/main.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp src/menu.cpp src/indiceInvertido.cpp calculo/palindromo.cpp calculo/fx.cpp -o pgm` | `./pgm -u admin -p 1234 -f data/MatrizA.txt` |
| **Multiplicador de matrices** | `g++ -std=c++17 calculo/mainMatrices.cpp calculo/matrices.cpp -o multi` | `./multi` (el menú lo invoca con sus argumentos) |
| **Administrador de usuarios** | `g++ -std=c++17 administrador/mainAdmin.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp -o admin` | `./admin` (desde el menú, opción 1) |
| **Servidor del juego** | `g++ -std=c++17 -Wall -pthread juego/mainServidor.cpp juego/servidor.cpp juego/comun.cpp src/config.cpp -o servidor` | `./servidor` |
| **Cliente del juego** | `g++ -std=c++17 -Wall juego/mainCliente.cpp juego/comun.cpp src/config.cpp -o cliente` | `./cliente` |
