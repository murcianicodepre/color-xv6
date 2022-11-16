#include "types.h"
#include "user.h"
#include "graphics.h"

/* Prueba de planificación con prioridades */

int main(int argc, char** argv){
    //setprio(getpid(), HIGH);
    switch(fork()){
        case -1 : { exit(1); }
        case 0 : {
            /*
            char* argv[] = {"date", '\0'};
            exec("date", argv);
            */
            //printf(1, "Hijo\n");
            exit(0);
        }
        default : {
            //setprio(getpid(), NORMAL);
            //printf(1, "Padre\n");
            exit(0);
        }
    }
    exit(0);
}