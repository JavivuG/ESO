# ESO: UVash y utilidades de Linux en C

Un shell de Unix (`UVash`) y versiones propias de `cat`, `grep`, `zip` y `unzip`, escritos en C sobre llamadas al sistema POSIX.

Asignatura **Estructura de Sistemas Operativos**, 2.º curso, 2.º cuatrimestre, curso 2022-2023. Grado en Ingeniería Informática, Universidad de Valladolid.

## Qué hace

| Práctica | Contenido |
| --- | --- |
| Práctica 1: utilidades Unix | `UVacat`, `UVagrep`, `UVazip` y `UVaunzip`: versiones simplificadas de `cat`, `grep` y de un compresor RLE con su descompresor. |
| Práctica 2: shell `UVash` | Intérprete de comandos con modo interactivo (prompt `UVash> `) y modo batch (lee órdenes de un fichero), comandos internos `exit` y `cd`, redirección de salida con `>` y ejecución en paralelo con `&`. |
| Examen de laboratorio (22/03/2023) | Ampliación de `UVash` con un comando interno `PACK` (ejercicio 1) y `UVafind`, un buscador de ficheros por nombre en el directorio actual con la opción `--name` (ejercicio 2). |
| Deberes (`homework/`) | Simuladores en Python del libro OSTEP sobre memoria: reubicación, segmentación, paginación lineal y multinivel, y `malloc`. |

Los enunciados, en inglés, están en el `README.md` de cada práctica. Proceden de los proyectos OSTEP de la Universidad de Wisconsin-Madison, adaptados para la asignatura.

## Tecnologías

- C (gcc, Linux)
- API POSIX: `fork`, `execvp`, `wait`, `dup2`, `chdir`, `opendir`/`readdir`
- Bash y el *tester* de OSTEP para las baterías de pruebas
- Python para los simuladores de `homework/`

## Lo que aprendí / Aspectos técnicos destacados

- **Creación y espera de procesos**: cada comando externo se lanza con `fork` + `execvp`. El shell espera a todos los hijos con `while (wait(NULL) > 0)`, así que los comandos separados por `&` se ejecutan a la vez y no uno tras otro.
- **Redirección con `dup2`**: el proceso hijo redirige la salida estándar y la de error al fichero indicado tras `>`. Antes, el shell rechaza las redirecciones mal formadas, como la que no indica fichero o la que indica varios.
- **Análisis de la línea**: primero se separan con `strsep` los comandos paralelos (`&`) y después los argumentos. Se normalizan los espacios y tabuladores sobrantes, y los casos límite (una línea con solo `&` o solo espacios) generan un único mensaje de error por `stderr`.
- **Comandos internos sin `fork`**: `exit` y `cd` se resuelven en el propio proceso del shell, porque un hijo no podría cambiar el directorio de trabajo de su padre.
- **Compresión binaria**: `UVazip` escribe cada tramo como un entero de 4 bytes seguido del carácter (`fwrite`), que es el formato que espera `UVaunzip`.

## Estructura del repositorio

```text
ESO/
├── homework/                  # Simuladores OSTEP de memoria (Python)
└── laboratorio-eso/
    ├── initial-reverse/       # Enunciado y tests de calentamiento (sin solución)
    ├── Practica1/
    │   ├── UVacat/  UVagrep/  UVazip/  UVaunzip/   # Fuente .c, test-*.sh y tests/
    ├── Practica2/
    │   ├── UVash.c            # Shell
    │   ├── test-UVash.sh  tests/
    │   ├── estudioExamen/     # Versiones de preparación del examen
    │   └── modificacionExamenESO/   # Examen: PACK (ejercicio1) y UVafind (ejercicio2)
    └── tester/                # Framework de pruebas de OSTEP
```

## Requisitos

- Linux (o WSL) con `gcc` y `bash`.
- Python 2 para `paging-linear-translate.py`, `paging-multilevel-translate.py` y `malloc.py`, que usan `print` como sentencia.

## Cómo compilar y ejecutar

Cada programa se compila con `gcc` y se prueba con el script `test-*.sh` de su carpeta, que ejecuta los casos de `tests/` con `../tester/run-tests.sh` (`-c` continúa tras un fallo):

```sh
cd laboratorio-eso/Practica2
gcc -Wall -o UVash UVash.c
./test-UVash.sh          # batería de pruebas
./UVash                  # modo interactivo
./UVash fichero.txt      # modo batch

cd ../Practica1/UVacat
gcc -o UVacat UVacat.c
./test-UVacat.sh
```

Para las demás utilidades, cambia el nombre (`UVagrep`, `UVazip`, `UVaunzip`).

Resultado comprobado al compilar con gcc en Linux (WSL), sobre una copia del repositorio: `UVash` pasa 22/22 tests y `UVacat` 7/7; `UVagrep` pasa 5/7, `UVazip` 4/6 y `UVaunzip` 1/6.

## Autoría

Javier García González (JavivuG).

---

Parte de [UVa-Projects](https://github.com/JavivuG/UVa-Projects).
