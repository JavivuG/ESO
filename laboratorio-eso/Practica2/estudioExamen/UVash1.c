// TRABAJO REALIZADO POR:
// GARCÍA GONZÁLEZ, JAVIER
// UVash.c
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define PROMPT "UVash> "
#define MAXARGS 256

void mensajeDeError();
void quitarSalto(char *palabra);
void formateaLinea(char *linea);
void borraEspaciosExtra(char *cadena);
void ejecutaComandos(char **argumentos, int nargs, bool *sigue, bool redireccion,bool *comandoBuiltin,bool *moverArchivoComandos ,FILE *fout);
void tokenizaArgumentos(char *tokenParalelo, char **argumentos, int *nargs, int *indiceRedireccion,bool*redireccion, bool *ejecutar, FILE **fout);
void cuentaGuiones(char *tokenParalelo, bool *guiones);

int main (int argc, char *argv[]){
  int nargs=0, indiceRedireccion=-2;
  char *linea, *tokenParalelo;
  char **argumentos;
  size_t size;
  bool modoBatch=false, sigue=true, redireccion=false, ejecutar=true, ejecutarLinea=true, comandoBuiltin, moverArchivoComandos=false;
  FILE *fin; // Fichero de entrada modo interactivo
  FILE *fout; // Fichero de salida en redirreccion
  FILE *ficheroComandos; // Fichero que guarda los comandos que se van ejecutando

  if (argc==2){
    modoBatch=true;
    if ((fin=fopen(argv[1], "r"))==NULL){ // Si se da un fichero batch incorrecto
      mensajeDeError();
      exit(1);
    }
  }
  else if (argc > 2) { // Si se indica mas de un fichero para el modo batch
    mensajeDeError();
    exit(1);
  }

  // Si lo que quiero es que se vaya acumulando todos los comandos que se han ejecutado historicamente cambio "w" por "a"
  if ((ficheroComandos=fopen("ficheroComandos.txt", "w"))==NULL){ // Abro el archivo que va almacenando los comandos que se ejecutan
    mensajeDeError();
  }
  
  while(sigue){
    if (modoBatch){ // Cuando esta en modo batch
      getline(&linea, &size, fin);
      if (feof(fin)) { // Fin del archivo
        free(linea);
        fclose(fin);
        exit(0);
      }
    }
    else{ // Cuando esta en modo interactivo
      write(1,PROMPT,strlen(PROMPT));

      getline(&linea, &size, stdin);
    }

    formateaLinea(linea); // Quito salto de linea y espacios extra

    // Reservo memoria para los argumentos y el comando
    if ((argumentos=(char**)malloc(sizeof(char*)*MAXARGS))==NULL){
      mensajeDeError();
      exit(1);
    } 
  
    if (strcmp(linea,"&")==0){ // Si la linea solo es "&" entonces es erroneo
      mensajeDeError();
      ejecutarLinea=false;
    }

    if (strcmp(linea,"")==0){ // Si introduce una cadena de solo espacios
      ejecutar=false;
    }

    if (ejecutarLinea){
      fprintf(ficheroComandos, "%s", linea); // escribo los comandos en el fichero que guarda los comandos
      
      while((tokenParalelo=strsep(&linea,"&"))!=NULL){ // Separo por "&" para ejecutar los comandos paralelos
        formateaLinea(tokenParalelo); // Quito espacios extra

        if (strcmp(tokenParalelo,"\0")==0){ // Si la cadena esta vacia no ejecuto ningun comando
          ejecutar=false;
        }
   
        // Tokenizo los argumentos del comando
        // Ademas de tokenizar tambien se guardan los argumentos y el programa en un array de strings y detecto redireccion 
        tokenizaArgumentos(tokenParalelo, argumentos, &nargs, &indiceRedireccion, &redireccion, &ejecutar, &fout);  

        if (ejecutar){
          // Ejecuto el comando ya sea un BUILT-IN o del resto
          ejecutaComandos(argumentos, nargs, &sigue, redireccion,&comandoBuiltin,&moverArchivoComandos, fout);

          if (comandoBuiltin){
            fprintf(ficheroComandos, " --COMANDO BUILT-IN\n");
          }
          else {
            fprintf(ficheroComandos, " --COMANDO DEL SISTEMA\n");
          }
          comandoBuiltin=false;

          if (redireccion){ // Si hay redireccion cierro archivo de salida
            fclose(fout);
          }
        }

        // Libero memoria dinamica
        for (int i=0; i<(nargs+1); i++){
          free(argumentos[i]);
        }

        // Reseteo las variables
        nargs=0;
        ejecutar=true;
        indiceRedireccion=-2; // Es -2 para que cuando no haya redireccion no haya problema al comparar con el contador
        redireccion=false;

      } // Bucle de cada token para comandos paralelos

      while((wait(NULL))>0); // Espera a que acaben de ejecutarse todos los comandos
    }
    else {
      ejecutarLinea=true;
    }

    // Libero memoria dinamica 
    free(argumentos);
    free(linea);
 
  } // Fin del bucle de sigue del programa principal
  
  if (moverArchivoComandos){
    if (rename("ficheroComandos.txt","DirectorioExamen/ficheroComandos.txt")==-1){
      mensajeDeError(); 
    }
  }
  fclose(ficheroComandos);
  exit(0);
} // Fin del main

void mensajeDeError(){ // Imprime el mensaje de error por stderr
  char error_message[30] = "An error has occurred\n";
  fprintf(stderr, "%s", error_message);
}

void quitarSalto(char *palabra){ // Quita el salto de linea de una palabra
  if (palabra[strlen(palabra)-1]=='\n'){
    palabra[strlen(palabra)-1]='\0';
  }
}

void borraEspaciosExtra(char *cadena) { // Tambien borra los tabuladores '\t'
  int i, j;
  int tam = strlen(cadena);
  bool espacioEncontrado = true;

  // Eliminar espacios al principio de la cadena
  while (cadena[0] == ' ' || cadena[0]=='\t') {
    for (i = 0; i < tam; i++) {
      cadena[i] = cadena[i+1];
    }
    tam--;
  }
  
  // Eliminar espacios extra en el resto de la cadena dejando un unico espacio
  for (i = 0, j = 0; i < tam; i++) {
      if (cadena[i] == ' '){
        if (!espacioEncontrado) {
            cadena[j] = ' ';
            j++;
            espacioEncontrado = true;
        }
      }
      else if(cadena[i]=='\t'){
        if (!espacioEncontrado) {
            cadena[j] = '\t';
            j++;
            espacioEncontrado = true;
        }
      }
      else {
        cadena[j] = cadena[i];
        j++;
        espacioEncontrado = false;
      }
  }
  
  // Elimina espacios extra al final de la cadena
  while (j > 0 && (cadena[j-1] == ' ' || cadena[j-1] == '\t')) {
    j--;
  }
  cadena[j] = '\0';
}

void formateaLinea(char *linea){ // Cuenta los argumentos que se introducen al programa
  quitarSalto(linea);
  borraEspaciosExtra(linea);
}

void ejecutaComandos(char **argumentos, int nargs, bool *sigue, bool redireccion,bool *comandoBuiltin, bool *moverArchivoComandos,FILE *fout){
  int pid, estado;

  // Comandos BUILT-IN
  if (strcmp(argumentos[0], "exit")==0){
    if (nargs==0){ // Para que salga del programa
      *sigue=false;
      *comandoBuiltin=true;
    }
    else{ // Si tiene 1 o mas argumentos error
      mensajeDeError();  
    }
  }
  else if (strcmp(argumentos[0], "cd")==0) {
    if (nargs==0 || nargs>1) { // Si tiene 0 o mas de 1 argumento error
      mensajeDeError();
    }
    else {
      *comandoBuiltin=true;
      if(chdir(argumentos[1])==-1){ // Cambio al directorio indicado
        mensajeDeError(); // Si ha surgido un error en la llamada a chdir
      }
    }
  }
  else if (strcmp(argumentos[0], "HazDir")==0){ // Crea un directorio si se ejecuta el comando HazDir
    if (nargs==0){
      *comandoBuiltin=true;
      *moverArchivoComandos=true;
      if ((mkdir("./DirectorioExamen", 0755))==-1){
        mensajeDeError();
      }
      else {
        printf("Directorio creado correctamente.\n");
      }

    }
    else {
      mensajeDeError();
    }
  }
  else { // Si no es ningun metodo BUILT-IN 
    pid = fork();

    if (pid==-1){ // Error al crear proceso hijo
      mensajeDeError();
    }
    else if (pid==0) { // PROCESO HIJO
      if (redireccion){ // Si hay redireccion entonces cambio la salida al fichero de redireccion
        if (dup2(fileno(fout), 1)==-1 || dup2(fileno(fout), 2)==-1){ // Cambio la salida y la salida de error predeterminada
          mensajeDeError(); // Si hay error cambiando salidas lanzo mensaje
          exit(1);
        }
      }

      if ((argumentos[nargs+1]=(char*)malloc(sizeof(char)*8))==NULL){
        mensajeDeError();
      }
      argumentos[nargs+1]=NULL;
 
      *comandoBuiltin=false;
      if((estado=execvp(argumentos[0], argumentos))==-1){ // Ejecuto el comando deseado
        mensajeDeError();
        exit(1);
      }
    } // Fin del proceso hijo
  }
}

// Funcion que tokeniza los comandos y los mete en el array de strings argumentos detectando si hay o no redireccion
void tokenizaArgumentos(char *tokenParalelo, char **argumentos, int *nargs, int *indiceRedireccion,bool*redireccion, bool *ejecutar, FILE **fout){
  char redirec[]=">";
  char *token;
  int contador=0;
  bool guiones;
  int i,j;
  int longitud=strlen(tokenParalelo);

  // si se ponen dos guiones se debe ejecutar el comando time
  cuentaGuiones(tokenParalelo, &guiones);

  if (strchr(tokenParalelo, '-')!=NULL && guiones){
    for(i = 0; i < longitud; i++) {
      if(tokenParalelo[i] == '-' && tokenParalelo[i+1]=='-') {
          for(j = longitud; j > i+2; j--) {
              tokenParalelo[j] = tokenParalelo[j-1];
          }
          tokenParalelo[i+2] = ' ';
          longitud++;
      }
    }
    formateaLinea(tokenParalelo);
  }
  
  // tokeniza los argumentos
  while((token=strsep(&tokenParalelo," "))!=NULL){
    if (strcmp(token,redirec)!=0){ // Si no es el simbolo de redireccion '>'
      if (contador==(*indiceRedireccion+1)){ // Si es el fichero de salida
        if ((*fout=fopen(token, "w"))==NULL){ // Abro el fichero de la redireccion
          mensajeDeError();
        }  
        *redireccion=true;
      }
      else {
        if (*redireccion){ // Si hay mas de un fichero de redireccion error
          mensajeDeError();
          *ejecutar=false;
        }
        else {
          // Reservo memoria dinamica para cada posicion
          if ((argumentos[*nargs]=(char*)malloc(sizeof(char)*strlen(token)))==NULL){
            mensajeDeError();
            exit(1);
          }
          if (strstr(token,"--")!=NULL && guiones){
            strcpy(argumentos[*nargs],"time");
          }else {
            strcpy(argumentos[*nargs],token); // Si no es el '>' ni el fichero de redireccion entonces es un argumento y lo guardo
          }
          (*nargs)++; // Aumento la cantidad de argumentos
        }
      }
    }
    else { // Si es el simbolo de redireccion o el fichero '>'
      *indiceRedireccion=contador;
      if (*redireccion || contador==0){
        mensajeDeError();
        *ejecutar=false;
      }
    }
    contador++;
  } // Fin bucle de tokenizacion

  // nargs indica el numero de argumentos, es decir si se introduce "ls" nargs sera 0
  // Si se introduce "ls -al" entonces nargs sera 1
  (*nargs)--; 

  if (*indiceRedireccion==(contador-1) && !(*redireccion)){  // Si hay redireccion pero no se ha especificado archivo error
    mensajeDeError();
    *ejecutar=false;
  }

}

void cuentaGuiones(char *tokenParalelo, bool *guiones){
  int contador=0;
  for (int i=0; i<strlen(tokenParalelo); i++){
    if (tokenParalelo[i]=='-'){
      contador++;
    }
    if (contador>2){
      *guiones=false;
      break;
    }
  }
  if (contador==2){
    *guiones=true;
  }
}

