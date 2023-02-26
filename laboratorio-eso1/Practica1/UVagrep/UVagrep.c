#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Falta incorporar la stdin
int main(int argc, char *argv[]) {
  FILE *fin;
  char fline[255];
  int counter = 2;
  char *newline;

  if (argc < 2) {
    fprintf(stdout, "UVagrep: searchterm [file ...]\n");
    exit(1);
  }

  while (counter <= (argc - 1)) {
    if ((fin = fopen(argv[counter], "r")) == NULL) {
      fprintf(stdout, "UVagrep: cannot open file\n");
      exit(1);
    }
    counter++;

    while (fgets(fline, sizeof(fline), fin) != NULL) {
      if ((newline = strchr(fline, '\n')) != NULL) {
        *newline = '\0';
      }
      if (strstr(fline, argv[1]) != NULL) {
        printf("%s\n", fline);
      }
    }
    fclose(fin);
  }

  exit(0);
}
