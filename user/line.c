#include "types.h"
#include "user.h"
#include "graphics.h"

/* Prueba de llamada al sistema drawlines */

void printUso(char* argv0){
    printf(2, "Uso: %d [AX] [AY] [BX] [BY] [color] \n");
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

int main(int argc, char** argv){
    /* Esperamos 5 argumentos, luego si argc<6 abortamos */
    if(argc<6){ printUso(argv[0]); exit(1); }
    else{
        int ax =  atoi(argv[1]), ay = atoi(argv[2]), bx = atoi(argv[3]), by = atoi(argv[4]), color = parse_color(argv[5]);
        if(drawline(ax, ay, bx, by, color, BLOCK) < 0){ printf(2, "drawline failed!"); exit(1); }
    }
    exit(0);
}