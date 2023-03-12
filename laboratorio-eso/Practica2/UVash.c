// TRABAJO REALIZADO POR:
// GARCÍA GONZÁLEZ, JAVIER
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define PROMPT "UVash> "
#define MAXARGS 512

void mensajeDeError();
void quitarSalto(char *palabra);
void formateaLinea(char *linea);
void borraEspaciosExtra(char *cadena);
void ejecutaComandos(char **argumentos, int nargs, bool *sigue, bool redireccion,FILE *fout, char *linea);
void liberaMemoriaDinamica(char **argumentos, char *linea, int nargs);

int main (int argc, char *argv[]){
  int indiceRedireccion;
  char *linea, *token;
  char redirec[]=">";
  char **argumentos;
  size_t size;
  int contador, nargs;
  bool modoBatch=false, sigue=true, redireccion, ejecutar;
  FILE *fin; // Fichero de entrada modo interactivo
  FILE *fout; // Fichero de salida si hay redirreccion

  if (argc==2){
    modoBatch=true;
    if ((fin=fopen(argv[1], "r"))==NULL){
      mensajeDeError();
      exit(1);
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
    ejecutar=true;
    indiceRedireccion=-2;

    if (modoBatch){ // Cuando esta en modo bath
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

    formateaLinea(linea);
    if (strcmp(linea,"")==0){
      ejecutar=false;
    }

    // Reservo memoria para los argumentos y el comando
    if ((argumentos=(char**)malloc(sizeof(char*)*MAXARGS))==NULL){
      mensajeDeError();
      exit(1);
    } 

    // Tokenizo los argumentos del comando
    while((token=strsep(&linea," "))!=NULL){
      if (strcmp(token,redirec)!=0){ // Si no es el simbolo de redireccion
        if (contador==(indiceRedireccion+1)){ // Si es el fichero de salida
          if ((fout=fopen(token, "w"))==NULL){
            mensajeDeError();
          }  
          redireccion=true;
        }
        else {
          if (redireccion){ // Si hay mas de un fichero de redireccion
            mensajeDeError();
            ejecutar=false;
          }
          else {
            // Reservo memoria dinamica para cada posicion
            if ((argumentos[nargs]=(char*)malloc(sizeof(char)*strlen(token)))==NULL){
              mensajeDeError();
              exit(1);
            }
            strcpy(argumentos[nargs],token);
            nargs++;
          }
        }
      }
      else { // Si es el simbolo de redireccion o el fichero
        indiceRedireccion=contador;
        if (redireccion || contador==0){
          mensajeDeError();
          ejecutar=false;
        }
      }
      contador++;
    } // Fin bucle de tokenizacion

    if (indiceRedireccion==(contador-1) && !redireccion){
      mensajeDeError();
      ejecutar=false;
    }
    
    if ((argumentos[nargs+1]=(char*)malloc(sizeof(char)*8))==NULL){
      mensajeDeError();
      exit(1);
    }

    nargs--;

    // COMANDOS BUILT-IN
    if (ejecutar){
      ejecutaComandos(argumentos, nargs, &sigue, redireccion, fout, linea);
    }
    if (redireccion){
      fclose(fout);
    }
    liberaMemoriaDinamica(argumentos, linea,nargs);
    
  } // Fin del bucle de sigue del programa principal
  exit(0);
}

void mensajeDeError(){ // Imprime el mensaje de error por stderr
  char error_message[30] = "An error has occurred\n";
  fprintf(stderr, "%s", error_message);
}

void quitarSalto(char *palabra){ // Quita el salto de linea de una palabra
  palabra[strlen(palabra)-1]='\0';
}

void formateaLinea(char *linea){ // Cuenta los argumentos que se introducen al programa
  quitarSalto(linea);
  borraEspaciosExtra(linea);
}

void borraEspaciosExtra(char *cadena) {
  int i, j;
  int tam = strlen(cadena);
  int espacioEncontrado = 0;

  // Eliminar espacios al principio de la cadena
  while (cadena[0] == ' ') {
    for (i = 0; i < tam; i++) {
        cadena[i] = cadena[i+1];
    }
    tam--;
  }
  
  // Eliminar espacios extra en el resto de la cadena
  for (i = 0, j = 0; i < tam; i++) {
      if (cadena[i] == ' ') {
          if (!espacioEncontrado) {
              cadena[j] = ' ';
              j++;
              espacioEncontrado = 1;
          }
      } else {
          cadena[j] = cadena[i];
          j++;
          espacioEncontrado = 0;
      }
  }
  
  // Elimina espacios extra al final de la cadena
  while (j > 0 && cadena[j-1] == ' ') {
    j--;
  }
  cadena[j] = '\0';
}

void ejecutaComandos(char **argumentos, int nargs, bool *sigue, bool redireccion,FILE *fout, char *linea){
  int pid, estado;
  if (strcmp(argumentos[0], "exit")==0){
    if (nargs==0){
      *sigue=false;
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
      argumentos[nargs+1]=NULL; // Execvp necesita que la ultima posicion sea NULL
      if((estado=execvp(argumentos[0], argumentos))==-1){
        mensajeDeError();
        exit(1);
      }

    } // Fin del proceso hijo
    else { // PROCESO PADRE
      wait(NULL);  
    }
  }
}

void liberaMemoriaDinamica(char **argumentos, char *linea, int nargs){
  for (int i=0; i<(nargs); i++){
    free(argumentos[i]);
  }
  free(argumentos);
  free(linea); 
}
