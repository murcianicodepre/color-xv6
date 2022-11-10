#include "types.h"
#include "user.h"
#include "date.h"
#include "graphics.h"

/* Cambia el color del texto: */

void printUsage(char* argv0){
    printf(1, "Uso: %s [COLOR]\n", argv0);
    printf(1, " %cBLACK", LGREY);
    printf(1, " %cBLUE", BLUE);
    printf(1, " %cDBLUE", DBLUE);
    printf(1, " %cGREY\n", GREY);
    printf(1, " %cGREEN", GREEN);
    printf(1, " %cDGREEN", DGREEN);
    printf(1, " %cCYAN", CYAN);
    printf(1, " %cDCYAN\n", DCYAN);
    printf(1, " %cYELLOW", YELLOW);
    printf(1, " %cRED", RED);
    printf(1, " %cLRED", LRED);
    printf(1, " %cBROWN\n", BROWN);
    printf(1, " %cMAGENTA", MAGENTA);
    printf(1, " %cDMAGENTA", DMAGENTA);
    printf(1, " %cWHITE", WHITE);
    printf(1, " %cLGREY\n", LGREY); 
}

int parse_color(char* color){
    // Parseamos la representación textual del color
    // Empezamos viendo si es color L, D u otro
    switch(color[0]){
        case 'D':           // Color oscuro: DBLUE, DGREEN y DMAGENTA
            switch(color[1]){
                case 'B': return 0x100;
                case 'G': return 0x200;
                case 'M': return 0x500;
            }
        case 'L':           // Color claro: LRED y LGREY
            switch(color[1]){
                case 'R': return 0xc00;
                case 'G': return 0x700;
            }
        case 'B':           // BLACK, BROWN o BLUE
            switch(color[1]){
                case 'L': 
                    switch(color[2]){
                        case 'U' : return 0x900;
                        case 'A' : return 0x000;
                    }
                case 'R': return 0x600;
            }
        case 'G':           // GREEN o GREY
            switch(color[3]){
                case 'E': return 0xa00;
                case 'Y': return 0x800;
            }
        case 'R':           // RED
            return 0x400;
        case 'C':           // CYAN
            return 0xb00;
        case 'M':           // MAGENTA
            return 0xd00;
        case 'Y':           // YELLOW
            return 0xe00;
        case 'W':           // WHITE
            return 0xf00;
    }
    return 0x700;
}

int main(int argc, char* argv[]){
    int c = 0x700;
    if(argc==1) { 
        color(0x700);
        printUsage(argv[0]);
        exit(1);
    }
    else c = parse_color(argv[1]);
    // Cambiamos el color mediante la llamada del sistema
    if(color(c) < 0) printUsage(argv[0]);
    exit(0);
}