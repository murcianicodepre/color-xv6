#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  /*
  char* a = sbrk (15000);

  a[500] = 1;   // Fallo de página
  
  if ((uint)a + 15000 != (uint) sbrk (-15000))
  {
    printf (1, "sbrk() con numero positivo fallo.\n");
    exit(1);
  }

  if (a != sbrk (0))
  {
    printf (1, "sbrk() con cero fallo.\n");
    exit(2);
  }

  if (a != sbrk (15000))
  {
    printf (1, "sbrk() negativo fallo.\n");
    exit(3);
  }

  printf (1, "Debe imprimir 1: %d.\n", ++a[500]);
  */
  char* a = sbrk(4096);
  printf(1, "a = %d\n", a);

  a[27] = 27;

  printf(1, "sbrk(-4096) = %d\n", sbrk(-4096));
  printf(1, "sbrk(4096) = %d\n", sbrk(4096));

  a[27] = 27;
  

  exit(0);
}
