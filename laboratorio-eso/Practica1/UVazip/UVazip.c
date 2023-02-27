#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NARG 2

int main(int argc, char *argv[]) {
  FILE *fin;
  int contador = 1;
  int tamanyo;
  char *linea;
  long size;

  if ((fin = fopen(argv[1], "r")) == NULL) {
    fprintf(stdout, "No se pudo abrir el fichero de entrada\n");
    exit(1);
  }

  if ((size = fseek(fin, 0L, SEEK_END)) != 0) { // Si no hay error devuelve
    fprintf(stdout, "No puedo ir al final del fichero de entrada\n");
    exit(1);
  }

  size = ftell(fin);

  if ((linea = (char *)malloc(size * sizeof(char))) == NULL) {
    fprintf(stdout, "No se pudo reservar memoria\n");
    fclose(fin);
    exit(1);
  }

  rewind(fin);

  if (fread(linea, size, 1, fin) != 1) {
    fprintf(stdout, "No se pudo leer todo el archivo\n");
    fclose(fin);
    exit(1);
  }

  fclose(fin);

  tamanyo = strlen(linea);

  for (int i = 0; i < (tamanyo - 1); i++) {
    if (linea[i] == linea[i + 1]) {
      contador++;
    } else {
      contador = 38;
      fwrite(&contador, sizeof(int), 1, stdout);
      printf("%c", linea[i]);
      contador = 1;
    }
  }
  fwrite(&contador, sizeof(int), 1, stdout);

  free(linea);
  exit(0);
}
