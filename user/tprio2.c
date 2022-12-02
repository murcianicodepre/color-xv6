#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  // Padre termina
  if (fork() != 0)
    exit(1);
  
  // Establecer prioridad normal. El shell aparecerá normalmente.
  setprio (getpid(), NORMAL);

  int r = 0;
  
  for (int i = 0; i < 2000; ++i)
    for (int j = 0; j < 1000000; ++j)
      r += i + j;

  // Imprime el resultado
  printf (1, "Resultado: %d\n", r);
  
  exit(1);
}