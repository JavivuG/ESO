#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  FILE *fin;
  int tamanyo, contador = 1, numFicheros = 1;
  size_t size = 0;
  char *linea = NULL;
  char caracter;

  if (argc < 2) {
    fprintf(stdout, "UVazip: file1 [file2 ...]\n");
    exit(1);
  }

  while (numFicheros < argc) {
    if ((fin = fopen(argv[numFicheros], "r")) == NULL) {
      fprintf(stdout, "UVazip: Cannot open file\n");
      exit(1);
    }

    while ((tamanyo = getline(&linea, &size, fin)) != -1) {
      
      for (int i = 0; i < tamanyo; i++) {
        if (linea[i] == linea[i + 1]) {
          contador++;
        } 
        else {
          if (argc==2){ // Si solo hay un fichero
            fwrite(&contador, sizeof(int), 1, stdout);
            fwrite(&linea[i], sizeof(char), 1, stdout);
            contador=1;
          }
          else {
            if (i!=(tamanyo-1)){
              fwrite(&contador, sizeof(int), 1, stdout);
              fwrite(&linea[i], sizeof(char), 1, stdout);
              contador=1;
            }
            else {
              caracter=linea[i];
            }
          }
        }
      }
    }

    numFicheros++;
    fclose(fin);
  }
  free(linea);

  return 0;
}
