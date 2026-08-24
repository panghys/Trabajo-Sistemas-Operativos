SistOpe - Administrador de Usuarios y Perfiles (Entrega 1)

1. Propósito de la aplicación
Esta primera entrega implementa un "Administrador de Usuarios y Perfiles". Su propósito principal es permitir la gestión (ingresar, listar y eliminar) de usuarios y perfiles del sistema. La aplicación utiliza estructuras de datos en memoria (structs) y garantiza la persistencia de la información almacenando los registros de forma dinámica en archivos de texto.

2. Cómo se debe ejecutar
El proyecto está desarrollado en C++ (estándar C++17) y utiliza la herramienta make para automatizar su compilación.

Paso 1: Compilar el código
**Opción A: Compilación automatizada (Si tienes "make" instalado)**
Abre una terminal en la raíz del proyecto y ejecuta el siguiente comando:
make

**Opción B: Compilación manual (Si NO tienes "make" instalado)**
Si tu sistema (como Windows PowerShell) no reconoce el comando "make", abre una terminal en la raíz del proyecto y ejecuta el siguiente comando con g++:
g++ -Wall -std=c++17 -Iinclude src/main.cpp src/config.cpp src/funcionesUsuarios.cpp src/funcionesPerfiles.cpp src/menu.cpp -o main.exe

Paso 2: Ejecutar el sistema
Una vez compilado con éxito, inicia el programa ejecutando el archivo generado:
.\main.exe  (En Windows PowerShell/CMD)
./main.exe  (En entornos Linux/macOS)

*(Nota: Asegúrate de tener creada la carpeta "data/" con los archivos de texto y el archivo ".env" en la raíz antes de ejecutar).*

3. Descripción de las variables de entorno
El sistema utiliza un archivo ".env" ubicado en la raíz del proyecto para cargar rutas dinámicas y evitar programar las direcciones en el código fuente. Las variables de entorno utilizadas son:

USER_FILE: Define la ruta exacta donde se encuentra el archivo de texto que almacena los registros de los usuarios (por ejemplo, "data/USUARIOS.TXT").
PERFIL_FILE: Define la ruta exacta del archivo de texto que almacena la información de los perfiles y sus opciones de menú permitidas (por ejemplo, data/PERFILES.TXT).