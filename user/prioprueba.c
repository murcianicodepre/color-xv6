#include "types.h"
#include "user.h"

int main(int argc, char** argv){

    setprio(getpid(), HIGH);

    for(int i=0; i<1000000; i++){}

    exit(0);
}