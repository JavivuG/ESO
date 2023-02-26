#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  FILE *fin;
  char *line;
  int counter = 1;

  while (counter <= (argc - 1)) {
    if ((fin = fopen(argv[counter], "r")) == NULL) {
      fprintf(stdout, "UVacat: no puedo abrir fichero\n");
      exit(1);
    }
    counter++;

    if ((line = (char *)malloc(sizeof(char) * 255)) == NULL) {
      fclose(fin);
      exit(1);
    }

    while (fgets(line, sizeof(line), fin)) {
      fprintf(stdout, "%s", line);
    }

    fclose(fin);
    free(line);
  }

  exit(0);
}
