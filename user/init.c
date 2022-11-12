// init: The initial user-level program
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "graphics.h"

char *argv[] = { "sh", 0 };

/* Intro 1 de xv6 */
void printIntro1(){
    /*
                                   00    00  00 000
          000 000  0   000  000     00  00  00 0000
         0   0   0 0  0   0 0  0     0000  00 00   00
         0   0   0 0  0   0 000     00  0000  00   00
          00  000  000 000  0  0   00    00    00000

          \n --> cambio de color
    */
  char* linea1 = "\n\n\n\n\n                                11   11  \n11 \n111\0";
  char* linea2 = "      111 \n111  \n1   \n111  \n1111     \n00 00  \n00 \n0011\0";
  char* linea3 = "     0   \n0   0 \n0  \n0   0 \n0   0     \n000  \n00 \n00   00\0";
  char* linea4 = "     0   \n0   0 \n0  \n0   0 \n0110     \n00 00\n00  \n00   00\0";
  char* linea5 = "      444 \n444  \n444 \n444  \n4   4   \n44   \n44    \n44444\0";
  char** lineas = (char**) malloc(sizeof(char*)*5);  
    lineas[0] = linea1; lineas[1] = linea2; lineas[2] = linea3; lineas[3] = linea4; lineas[4] = linea5;

  int estado = 0;   // 0 - rojo, 1 - marrón, 2 - amarillo, 3 - verde, 4 - azul, 5 - rojo, 6 - verde, 7 - azul
  printf(1, "\n");
  for(int i=0; i<5; i++){                               // Para cada línea...
      estado = 0;
      char* linea = lineas[i];  
      for(int j=0; j<strlen(linea); j++){               // Para cada caracter de la línea...
          char c = linea[j];
          char caracter = ' ';                          // Por defecto se pinta un espacio en blanco
          char color = LGREY;                           // Color con el que se va a pintar el caracter
          if(c=='\n') { estado++; }                     // Cambiamos al siguiente color          
          switch(c){                                    // ¿Qué bloque usamos para pintar?
            case '0' : { caracter = BLOCK; break; }
            case '1' : { caracter = HBLOCK; break; }
            //case '2' : { caracter = LBLOCK; break; }
            //case '3' : { caracter = RBLOCK; break; }
            case '4' : { caracter = UBLOCK; break; }
            //case '5' : { caracter = TBLOCK1; break; }
            //case '6' : { caracter = TBLOCK2; break; }
            //case '7' : { caracter = TBLOCK3; break; }
            //case '8' : { caracter = MBLOCK; break; }
            //case '9' : { caracter = CBLOCK; break; }
          }

          switch(estado){
            case 0 : { color = RED; break; }
            case 1 : { color = BROWN; break; }
            case 2 : { color = YELLOW; break; }
            case 3 : { color = GREEN; break; }
            case 4 : { color = BLUE; break; }
            case 5 : { color = RED; break; }
            case 6 : { color = GREEN; break; }
            case 7 : { color = BLUE; break; }
            default : { color = LGREY; break; }
          }

          if(c!='\n') printf(1, "%c%c", color, caracter);
      }
      printf(1, "\n");
  }
  printf(1, "%c      by diegojose.parragan@um.es\n\n", CYAN);
  free(lineas);
}


int
main(void)
{
  int pid, wpid, status;

  if(open("console", O_RDWR) < 0){
    mknod("console", 1, 1);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  for(;;){
    //printIntro1();
    printf(1, "%cinit: starting sh\n%c", YELLOW, LGREY);
    pid = fork();
    if(pid < 0){
      printf(1, "%cinit: fork failed\n%c", LRED, LGREY);
      exit(1);
    }
    if(pid == 0){
      exec("sh", argv);
      printf(1, "%cinit: exec sh failed\n%c", LRED, LGREY);
      exit(1);
    }
    while((wpid=wait(&status)) >= 0 && wpid != pid){ 
      printf(1, "%czombie!\n%c", CYAN, LGREY);
    }
  }
}
