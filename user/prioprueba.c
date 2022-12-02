#include "types.h"
#include "user.h"

int main(int argc, char** argv){

    if(fork()!=0){ exit(0); }

    //setprio(getpid(), HIGH);
    for(int i=0; i<27000000; i++){}

    exit(0);
}