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

void mensajeDeError();
void quitarSalto(char *palabra);
void cuentaArgumentos(char *linea, int *nargs);
void compruebaRedirreccion(char *linea, bool *redirreccion);

int main (int argc, char *argv[]){
  int pid, estado;
  char *linea;
  size_t size;
  int contador, nargs;
  bool modoBatch=false, sigue=true, redirreccion=false;
  char *token;
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

    if (modoBatch){
      getline(&linea, &size, fin);
      if(!feof(fin)){
        write(1,PROMPT,strlen(PROMPT));
        printf("%s", linea);
      }
      else if (feof(fin)) {
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

    // Quito saltos de linea de los comandos
    if (nargs==0){
      quitarSalto(argumentos[0]);
    }
    else {
      quitarSalto(argumentos[nargs]);
    }

    argumentos[nargs+1]=NULL;
    

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
			  if((estado=execvp(argumentos[0], argumentos))==-1){
				  mensajeDeError();
          exit(1);
			  }

      } // Fin del proceso hijo
      else { // PROCESO PADRE
        wait(NULL);
        for (int i=0; i<(nargs+1); i++){
          free(argumentos[i]);
        }
        free(argumentos);
        free(linea); 
      }
    }
  } // Fin del bucle de sigue del programa principal

  exit(0);
}

void mensajeDeError(){
  char error_message[30] = "An error has occurred\n";
  fprintf(stderr, "%s", error_message);
}

void quitarSalto(char *palabra){
  int longitud = strlen(palabra);
  if (longitud > 0 && palabra[longitud - 1] == '\n') {
    palabra[longitud - 1] = '\0';
  }
}

void cuentaArgumentos(char *linea, int *nargs){
  for (int i=0; i<strlen(linea); i++){
    if (linea[i]==' ' && linea[i+1]!=' '){
      (*nargs)=(*nargs)+1; // Si se introduce cat fichero1 fichero2 (nargs sera 2)
    }
  }
}

void compruebaRedirreccion(char *linea, bool *redirreccion){
  for (int i=0; i<strlen(linea); i++){
    if (linea[i]=='>'){
      *redirreccion=true;
    }
  }
}
