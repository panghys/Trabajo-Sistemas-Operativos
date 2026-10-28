SistOpe - Administrador de Usuarios y Perfiles

1. Propósito de la aplicación
Esta aplicación presenta al usuario con un menú con varias opciones, a las que se pueden acceder según se proporcione un cierto input y considerando los
permisos con los que cuente el usuario (admin o usuario general)

+ El programa tambien incluye la carpeta "LIBROS", que contiene 50 mbs en archivos de texto separados por generos.

2. Cómo se debe ejecutar
El proyecto está desarrollado en C++ (estándar C++17) y utiliza la herramienta make para automatizar su compilación.

### Paso 1: Compilar el código
Abre una terminal en la raíz del proyecto y ejecuta el siguiente comando:
make

### Paso 2: Ejecutar el sistema
Una vez se compila, hay que ejecutar en consola:

```.\pgm -u [usuario] -p [contraseña] -f [algún archivo]``` (Windows)

```./pgm -u [usuario] -p [contraseña] -f [algún archivo]``` (En linux o mac)

Donde [usuario] es el nombre de usuario, [contraseña] la clave del usuario y el path de algún archivo para ejecutar una de las funciones del programa, por ejemplo, data/MatrizA.txt

En caso de estar vacío el sistema, se generará automáticamente un usuario administrador. Luego de eso, se podrá ejecutar con:

``` ./pgm -u admin -p 1234 -f (archivo) ```


### Descripción de las variables de entorno
Las variables de entorno utilizadas son:

* USER_FILE: Ruta exacta donde se encuentra el archivo de texto que almacena los registros de los usuarios
* PERFIL_FILE: Ruta exacta del archivo de texto que almacena la información de los perfiles y sus opciones de menú permitidas
* LIBROS_DIR: Ruta exacta de donde se almacenan archivos de texto que contienen libros
* INDICE_FILE:  Ruta exacta donde se guardará el índice invertido.
* MAPA_LIBROS_FILE:  Ruta del archivo que enumera todos los libros, asociando cada libro con un id, este id correspondera con el almacenado en el indice invertido.
* N_THREADS: Número de hilos (threads) lógicos que se encargan de crear el índice invertido.

variables de entornos de juego

* GAME_HOST: Dirección IP del host.
* GAME_PORT: Puerto de red utilizado para la comunicación entre los jugadores.
* GAME_LOG_FILE: Ruta exacta del archivo que guarda el registro (log) de eventos del juego.
* GAME_BOARD_SIZE: Tamaño del tablero de juego.
* GAME_DICE_FACES: Número de caras del dado.

---

### Funcionalidades

1. Administracion de usuarios y perfiles: El acceso al programa requiere de un usuario, que a su vez cadaa usuario posee un perfil que permite bloquear ciertas opciones según sea el nivel de acceso del tipo de usuario (admin vs usuario). el prograama permite la creacion de nuevos perfiles y gestionar su nivel de acceso
2. Módulo de cálculo: El programa permite ejecutar funciones de cálculo, como una función f(x) específica, operaciones con matrices mediante la lectura de estas por medio de archivos. Ademas tambien permite funciones sobre texto, como saber si una palabra es palindromo y determinar la cantidad de palaabraas, caracteres, consonantes y caracteres especiaales de un texto o archivo
3. Creacion de modulo invertido: El programaa permite la creacion de un indice invertido de los libros que incluye el programa
4. Juego incorporado: El programa ahora tambien incluye un juego multijugador por turnos.
   Reglaas del juego:
        1. bla bla bla
        2. blu blu blu


### Funcionamiento tecnico de algunas funcionaliades

3. Indice invertido: Para la creacion del indice se capturaa cadaa texto plano (.txt) del directorio de libros se le asigna un id y se guarda en una colaa de trabajo. se procede a ejecutar un proceso en paralelo con una cantidad definida N_THREADS de hilos de trabajo, cada hilo solicita un elemento de laa colaa, esperando la señal de un semaforo que indica que laa cola este desupada paara evitar coliciones, una vez con su libro lo procesa y mezcla su resultado particulaaar a otra estrucutra que posee todos los resultados a nivel global, haci hasta que se aagote la cola. cabe recalcar que existen tres semaforos: uno para la cola, otro para la estructura qie posee los resultados globaales y una ultima para laa consola de salida.


4. Juego:
        
        
| Programa | Método | Comando de Compilación | Ejecución (Windows) | Ejecución (Linux / macOS) |
| :--- | :--- | :--- | :--- | :--- |
| **Menú Principal** | Automatizado (`make`) | `make` | `.\main.exe` | `./main.exe` |
| **Menú Principal** | Manual (`g++`) | `g++ -Wall -std=c++17 -Iinclude src/main.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp src/menu.cpp src/palindromo.cpp src/fx.cpp -o main.exe` | `.\main.exe` | `./main.exe` |
| **Multiplicador de Matrices** | Manual (`g++`) | `g++ -Wall -std=c++17 -Iinclude src/mainMatrices.cpp src/matrices.cpp -o multi.exe` | `.\multi.exe` | `./multi.exe` |

---
