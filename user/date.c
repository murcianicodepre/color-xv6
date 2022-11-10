#include "types.h"
#include "user.h"
#include "date.h"
#include "graphics.h"

/* Imprime la fecha del sistema */

int main(int argc, char** argv){
    struct rtcdate r;
    if(date(&r)){
        printf(2, "date failed\n");
        exit(1);
    }
    printf(1, "%c%d%c/%c%d%c/%c%d\n%c", LRED, r.day, MAGENTA, GREEN, r.month, MAGENTA,  BLUE, r.year, LGREY);
    exit(1);
}