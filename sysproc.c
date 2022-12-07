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
  exit(status<<8);                            // Dejamos el valor desplazado para que los macros funcionen
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

/* Lazy alloc: simulamos haber reservado las páginas, produciendo T_PGFLT y reservandolas entonces */
int sys_sbrk(void){
    int n, szanterior = myproc()->sz;
    if(argint(0, &n) < 0){ return -1; }
    if(n>=0){
        myproc()->sz += n;                      // No reservamos la página... todavía
    } else {                                    // Si reducimos el tamaño, deberíamos liberar las páginas
        if(growproc(n) < 0){ return -1; }
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
    if(c<0 || c>255) return -1;

    /* Update FOREGROUND_COLOR  */
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

/* Devuelve la prioridad del proceso con pid */
int sys_getprio(void){
    int pid;
    if(argint(0, &pid) < 0){ return -1; }
    if(pid<0) return -1;
    else return (int) getprocprio(pid);
}

/* Establece la prioridad de un proceso */
int sys_setprio(void){
    int pid;
    enum proc_prio prio;
    if(argint(0, &pid) < 0){ return -1; }
    if(argint(1, (int*)&prio) < 0){ return -1; }
    if(pid<0 || prio<0 || prio>2){ return -1; }
    setprocprio(pid, prio);
    return 0;
}