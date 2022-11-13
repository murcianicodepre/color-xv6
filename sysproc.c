#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  int status;
  if(argint(0, &status) < 0){ return -1; }    // Recuperamos el valor de salida de la pila del usuario
  exit(status);                               // El valor está en los 8 bits inferiores
  return 0;                                   // No debería alcanzarse
}

int
sys_wait(void)
{
  void* pstatus;
  if(argptr(0, &pstatus, sizeof(int*) < 0)){ return -1; }   // Recuperamos el puntero a status de la pila de usuario
  return wait((int*)pstatus);
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

/* Lazy alloc */
int sys_sbrk(void){
    int n, szanterior = myproc()->sz;
    if(argint(0, &n) < 0){ return -1; }
    if(n>=0){
        myproc()->sz += n;    // No reservamos la página... todavía
    } else {                  // Si reducimos el tamaño, deberíamos liberar las páginas
        if((myproc()->sz = deallocuvm(myproc()->pgdir, szanterior, szanterior + n)) == 0) { return -1; }
        lcr3(V2P(myproc()->pgdir));  // Invalidamos traducciónes del TLB
    }
    return szanterior;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

/* Actualiza la estructura rtcdate */
/* Devuelve la fecha actual del sistema */
int sys_date(void){
  struct rtcdate *r;                                         /* Argumento para recuperar de la pila del usuario */
  if(argptr(0, (void**)&r, sizeof(struct rtcdate)) < 0){     /* Recuperamos el argumento */
    return -1;
  }
  cmostime(r);
  return 0;
}

/* Changes text default color */
int sys_color(void){
    int c;
    /* Recover color from user stack */
    if(argint(0, &c) < 0){  
      return -1;
    }

    /* Check if color value is valid */
    if(c<0 || c>3840) return -1;

    /* Update CURRENT_COLOR with function */
    setColor(c);    
    return 0;
}

/* Devuelve el byte color del color actual que se está usando en la consola */
int sys_getcolor(void){
  return getColor();
}

/* Limpia la pantalla */
int sys_clear(void){
    clear();
    return 0;
}

/* Dibuja un buffer como imagen por pantalla */
int sys_draw(void){
    /* Recuperamos los 6 argumentos de la pila del usuario */
    int posx, posy, sx, sy;
    char* img; 
    char* color;
    if(argint(0, &posx) < 0){ return -1; }
    if(argint(1, &posy) < 0){ return -1; }
    if(argint(2, &sx) < 0){ return -1; }
    if(argint(3, &sy) < 0){ return -1; }
    if(argptr(4, (void**)&img, sx*sy) < 0){ return -1; }
    if(argptr(5, (void**)&color, sx*sy) < 0){ return -1; }

    /* Llamamos a la función de console.c dibujar en el frame buffer */
    draw(posx, posy, sx, sy, img, color);
    return 0;
}

/* Dibuja una línea entre dos puntos */
int sys_drawline(void){
    /* Recuperamos los argumentos */
    int ax, ay, bx, by, color, pixel;
    if(argint(0, &ax) < 0){ return -1; }
    if(argint(1, &ay) < 0){ return -1; }
    if(argint(2, &bx) < 0){ return -1; }
    if(argint(3, &by) < 0){ return -1; }
    if(argint(4, &color) < 0){ return -1; }
    if(argint(5, &pixel) < 0){ return -1; }

    /* Llamamos a la función de console.c */
    drawline(ax, ay, bx, by, color, pixel);
    return 0;
}