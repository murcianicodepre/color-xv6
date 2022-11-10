#include "types.h"
#include "user.h"

/* Limpia la pantalla */
int main(){
    if(clear() < 0) { exit(1); }
    else exit(0);
}