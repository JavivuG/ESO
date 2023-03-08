#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  FILE *fin;
  int tamanyo = 255;
  char *linea;
  size_t size = sizeof(int) + sizeof(char);
  if (argc < 2) {
    fprintf(stdout, "UVaunzip: file1 [file2 ...]\n");
    exit(1);
  }

  if ((fin = fopen(argv[1], "r")) == NULL) {
    fprintf(stdout, "UVaunzip: cannot open file1");
    exit(1);
  }

  if ((linea = (char *)malloc(sizeof(char) * tamanyo)) == NULL) {
    fprintf(stdout, "No se pudo reservar memoria");
    exit(1);
  }

  while (!feof(fin)) {
    fread(linea, size, 1, fin);
    printf("%s\n", linea);
  }
  free(linea);

  return 0;
}
