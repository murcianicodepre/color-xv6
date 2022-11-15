#include "types.h"
#include "user.h"
#include "graphics.h"

/* Prueba de llamadas al sistema sobre procesos y su prioridads */

int main(int argc, char** argv){
    int ocolor = getcolor();
    printf(1, "Este proceso es %c%d%c, con prioridad %c%d%c\n", MAGENTA, getpid(), ocolor, CYAN, getprio(getpid()), ocolor);
    if(setprio(getpid(), HIGH) < 0){ exit(1); }
    printf(1, "Este proceso es %c%d%c, con prioridad %c%d%c\n", MAGENTA, getpid(), ocolor, CYAN, getprio(getpid()), ocolor);
    exit(0);
}