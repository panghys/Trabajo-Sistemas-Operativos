SistOpe - Administrador de Usuarios y Perfiles (Entrega 1)

1. Propósito de la aplicación
Esta aplicación presenta al usuario con un menú con varias opciones, a las que se pueden acceder según se proporcione un cierto input y considerando los
permisos con los que cuente el usuario (admin o usuario general)
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
* LIBROs_DIR: Ruta exacta de donde se almacenan archivos de texto que contienen libros

Por motivos de la segunda entrega, se añadieron nuevas funcionalidades:

1. Módulo de cálculo: El programa permite ejecutar funciones de cálculo, como una función f(x) específica, operaciones con matrices mediante la lectura de estas por medio de archivos y una función que determina si cierto string es palíndromo
2. Acceso por roles: El programa ahora puede bloquear ciertas opciones según sea el nivel de acceso del tipo de usuario (admin vs usuario)
3. Menú expandido: El menú ahora presenta acceso a estas funciones (según rol) y designa el menú de la entrega uno (el de control de usuarios) a usuarios administradores


Además se añadió la carpeta "LIBROS", que contiene 50 mbs en archivos de texto.

| Programa | Método | Comando de Compilación | Ejecución (Windows) | Ejecución (Linux / macOS) |
| :--- | :--- | :--- | :--- | :--- |
| **Menú Principal** | Automatizado (`make`) | `make` | `.\main.exe` | `./main.exe` |
| **Menú Principal** | Manual (`g++`) | `g++ -Wall -std=c++17 -Iinclude src/main.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp src/menu.cpp src/palindromo.cpp src/fx.cpp -o main.exe` | `.\main.exe` | `./main.exe` |
| **Multiplicador de Matrices** | Manual (`g++`) | `g++ -Wall -std=c++17 -Iinclude src/mainMatrices.cpp src/matrices.cpp -o multi.exe` | `.\multi.exe` | `./multi.exe` |

---
