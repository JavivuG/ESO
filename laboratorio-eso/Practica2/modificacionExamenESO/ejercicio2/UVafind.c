// TRABAJO REALIZADO POR:
// GARCÍA GONZÁLEZ, JAVIER
// GRUPO DE LABORATORIO: X7
// EXAMEN DE LABORATORIO - DIA 22/03/2023 - EJERCICIO 2
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

// Imprimo el error de distinto numero de argumentos o si no se usa bien "--name"
void imprimeMensajeErrorArgumentos();

int main (int argc, char *argv[]){
  DIR *directorio;
  struct dirent *entradas;

  if (argc!=3 && argc!=4){ // Compruebo si tiene 2 o 3 argumentos
    imprimeMensajeErrorArgumentos();
    exit(1);
  }

  if (argc==3){ // 2 argumentos
    if (strcmp(argv[1], "--name")!=0){ // Compruebo si usa debidamente el flag "--name"
      imprimeMensajeErrorArgumentos();
    }

    if ((directorio = opendir(".")) != NULL) { // Abro el directorio actual

      while ((entradas = readdir(directorio)) != NULL) { // Leo las entradas que tiene el directorio actual

        // Compruebo si los nombres de los ficheros de las entradas son iguales al que busco
        if (strcmp(argv[2], entradas->d_name)==0){ 
          printf("%s\n", entradas->d_name); // Si lo encuentro entonces imprimo el nombre del fichero
        }  
      }
      closedir(directorio); // Cierro el directorio
    } 
  }
  else if (argc==4){ // 3 argumentos
    if (strcmp(argv[2], "--name")!=0){ // Compruebo que la flag se usa debidamente
      imprimeMensajeErrorArgumentos();
    }

    if ((directorio = opendir(argv[1])) != NULL) { // Abro el directorio indicado

      while ((entradas = readdir(directorio)) != NULL) { // Leo las entradas del directorio indicado
        
        // Compruebo si los nombres de los ficheros de las entradas son iguales al que busco
        if (strcmp(argv[3], entradas->d_name)==0){
          printf("%s\n", entradas->d_name); // Si lo encuentro entonces imprimo el nombre del fichero
        }  

      }

      closedir(directorio); // Cierro el directorio
    } 
    else { // Si no se puede abrir el directorio indicado entonces hay error
      fprintf(stderr,"UVafind: cannot open directory\n");
      exit(1);
    }  
  }

  exit(0);
}

void imprimeMensajeErrorArgumentos(){
  fprintf(stderr, "UVafind [dir] --name searchterm\n");
  exit(1);
}
