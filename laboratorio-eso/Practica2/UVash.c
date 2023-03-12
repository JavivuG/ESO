// TRABAJO REALIZADO POR:
// GARCÍA GONZÁLEZ, JAVIER
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define PROMPT "UVash> "

void liberaMemoriaDinamica(char **argumentos, char *linea, int nargs);
void mensajeDeError();
void quitarSalto(char *palabra);
void cuentaArgumentos(char *linea, int *nargs);
void borraEspaciosExtra(char *linea);
int compruebaRedireccion(char **argumentos, int nargs);

int main (int argc, char *argv[]){
  int pid, estado, indiceRedireccion;
  char *linea;
  size_t size;
  int contador, nargs;
  bool modoBatch=false, sigue=true, redireccion;
  char *token;
  char redirec[]=">";
  char **argumentos;
  FILE *fin; // Fichero de entrada modo interactivo
  FILE *fout; // Fichero de salida si hay redirreccion

  if (argc==2){
    modoBatch=true;
    if ((fin=fopen(argv[1], "r"))==NULL){
      mensajeDeError();
    }
  }
  else if (argc > 2) { // Si se indica mas de un fichero para el modo batch
    mensajeDeError();
    exit(1);
  }
  
  while(sigue){
    nargs=0; // Reseteo las variables
    contador=0;
    redireccion=false;

    if (modoBatch){
      getline(&linea, &size, fin);
      if (feof(fin)) {
        free(linea);
        fclose(fin);
        exit(0);
      }
    }
    else{ // Cuando esta en modo interactivo
      write(1,PROMPT,strlen(PROMPT));

      getline(&linea, &size, stdin);
    }

    cuentaArgumentos(linea, &nargs);

    // Reservo memoria para los argumentos y el comando
    if ((argumentos=(char**)malloc(sizeof(char*)*(nargs+2)))==NULL){
      mensajeDeError();
      exit(1);
    } 

    // Tokenizo los argumentos del comando
    while((token=strsep(&linea," "))!=NULL){
      // Reservo memoria dinamica para cada posicion
      if ((argumentos[contador]=(char*)malloc(sizeof(char)*strlen(token)))==NULL){
        mensajeDeError();
        exit(1);
      }
      strcpy(argumentos[contador],token);
      contador++;
    } // Fin bucle de tokenizacion


    argumentos[nargs+1]=NULL; // Execvp necesita que la ultima posicion sea NULL


    if ((indiceRedireccion=compruebaRedireccion(argumentos,nargs+1))!=-1){ // Comprueba que sea una redirreccion
      redireccion=true;
      if ((fout=fopen(argumentos[indiceRedireccion+1], "w"))!=NULL){
        mensajeDeError();
      }
    }


    // COMANDOS BUILT-IN
    if (strcmp(argumentos[0], "exit")==0){
      if (nargs==0){
        sigue=false;
      }
      else{ // Si tiene 1 o mas argumentos error
        mensajeDeError();  
      }
    }
    else if (strcmp(argumentos[0], "cd")==0) {
      if (nargs==0 || nargs>1) { // Si tiene 0 o mas de 1 argumento
        mensajeDeError();
      }
      else {
        if(chdir(argumentos[1])==-1){ // Si ha surgido un error en la llamada a chdir
          mensajeDeError();
        }
      }
    }
    else { // Si no es ningun metodo BUILT-IN 
      pid = fork();

      if (pid==-1){ // Error al crear proceso hijo
        mensajeDeError();
      }
      else if (pid==0) { // PROCESO HIJO
        if (redireccion){
          if (dup2(fileno(fout), 1)==-1 || dup2(fileno(fout), 2)==-1){
            mensajeDeError();
            exit(1);
          }
        }
			  if((estado=execvp(argumentos[0], argumentos))==-1){
				  mensajeDeError();
          exit(1);
			  }

      } // Fin del proceso hijo
      else { // PROCESO PADRE
        wait(NULL);
        
        if (redireccion){
          fclose(fout);
        }
        liberaMemoriaDinamica(argumentos,linea, nargs);
      }
    }
  } // Fin del bucle de sigue del programa principal
  exit(0);
}

void liberaMemoriaDinamica(char **argumentos, char*linea, int nargs){
  // Libero memoria dinamica
  for (int i=0; i<(nargs+2); i++){
    free(argumentos[i]);
  }
  free(argumentos);
  free(linea); 
}

void mensajeDeError(){ // Imprime el mensaje de error por stderr
  char error_message[30] = "An error has occurred\n";
  fprintf(stderr, "%s", error_message);
}

void quitarSalto(char *palabra){ // Quita el salto de linea de una palabra
  palabra[strlen(palabra)-1]='\0';
}

void cuentaArgumentos(char *linea, int *nargs){ // Cuenta los argumentos que se introducen al programa
  quitarSalto(linea);
  borraEspaciosExtra(linea);
  for (int i=0; i<strlen(linea); i++){
    if (linea[i]==' ' && linea[i+1]!=' '){
      (*nargs)=(*nargs)+1; // Si se introduce cat fichero1 fichero2 (nargs sera 2)
    }
  }
}

void borraEspaciosExtra(char *linea) {
  int i=0, j=0;
  int flag = 0;  // Para saber si se ha encontrado el primer espacio o tabulacion


  for (i = 0; i < strlen(linea); i++) { 
    if (linea[i] == ' ' || linea[i] == '\t') {
      if (flag == 0) {
          linea[j++] = linea[i];
          flag = 1;
      }
    } else {
      linea[j++] = linea[i];
      flag = 0;
      
    }
  }
  linea[j] = '\0';
}

int compruebaRedireccion(char **argumentos, int nargs){
  for (int i=0; i<nargs; i++){
    for (int j=0; j<strlen(argumentos[i]); j++){
      if (argumentos[i][j]=='>'){
        return i;
      }
    }
  }
  return -1;
}

