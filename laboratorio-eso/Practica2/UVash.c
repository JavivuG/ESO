// TRABAJO REALIZADO POR:
// GARCÍA GONZÁLEZ, JAVIER
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#define PROMPT "UVash> "

void mensajeDeError();
void quitarSalto(char *palabra);

int main (int argc, char *argv[]){
  char *linea;
  size_t size;
  int contador, nargs;
  bool modoBatch=false;
  char *token, *comando;
  char **argumentos;

  if (argc==3){
    modoBatch=true;
  }
  else if (argc > 3) { // Si se indica mas de un fichero para el modo batch
    mensajeDeError();
    exit(1);
  }
  
  while(true){
    nargs=0;
    contador=0;
    write(1,PROMPT,strlen(PROMPT));
    
    getline(&linea, &size, stdin);

    // Cuento el numero de argumentos que se introducen sin contar el propio comando
    for (int i=0; i<strlen(linea); i++){
      if (linea[i]==' ' && linea[i+1]!=' '){
        nargs++; // Si se introduce cat fichero1 fichero2 (nargs sera 2)
      }
    }

    // Reservo memoria para los argumentos
    if ((argumentos=(char**)malloc(sizeof(char*)*nargs))==NULL){
      mensajeDeError();
      exit(1);
    } 

    // Tokenizo los argumentos del comando
    while((token=strsep(&linea," "))!=NULL){
      if (contador==0){
        if ((comando=(char*)malloc(sizeof(char)*strlen(token)))==NULL){
          mensajeDeError();
          exit(1);
        }
        strcpy(comando, token);
        quitarSalto(comando);
      }
      else {
        if ((argumentos[contador-1]=(char*)malloc(sizeof(char)*strlen(token)))==NULL){
          mensajeDeError();
          exit(1);
        }
        strcpy(argumentos[contador-1], token);
      }
      contador++;
    } // Fin bucle de tokenizacion

    // COMANDOS BUILT-IN
    if (strcmp(comando, "exit")==0){
      if (nargs==0){
        exit(0);
      }
      else{ // Si tiene 1 o mas argumentos error
        mensajeDeError();  
      }
    }
    else if (strcmp(comando, "cd")==0) {
      if (nargs==0 || nargs>1) { // Si tiene 0 o mas de 1 argumento
        mensajeDeError();
      }
      else {
        if(chdir(argumentos[0])==-1){ // Si ha surgido un error en la llamada a chdir
          mensajeDeError();
        }
      }
    }
    
    for (int i=0; i<nargs; i++){
      free(argumentos[i]);
    }
    free(argumentos);
    free(comando);
    free(linea);
  } // Fin del bucle de sigue del programa principal


  return 0;
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
