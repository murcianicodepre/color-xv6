#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"
#include "graphics.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);

  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
  int ocolor = getColor();
  if(tf->trapno == T_SYSCALL){
    if(myproc()->killed)
      exit(1);
    myproc()->tf = tf;
    syscall();
    if(myproc()->killed)
      exit(1);
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(cpuid() == 0){
      acquire(&tickslock);
      ticks++;
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpuid(), tf->cs, tf->eip);
    lapiceoi();
    break;
  
  /* Manejador de fallos de página */
  case T_PGFLT : {     
        /* Fallo de página del usuario pero en kernel. Panic */         
        if(myproc()==0){
            cprintf("%c", RED);
            cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
                    tf->trapno, cpuid(), tf->eip, rcr2());
            panic("trap");
        }
        uint verr = PGROUNDDOWN(rcr2());    
        uint err = myproc()->tf->err;       

        /* ¿Acceso a kernel o fuera del program break? Muerte */
        if(PGROUNDUP(rcr2()) > myproc()->sz || verr >= KERNBASE){  
            if(verr >= KERNBASE) cprintf("%caddr%c 0x%x%c: illegal memory access -- kill proc\n%c", RED, CYAN, rcr2(), RED, ocolor); 
            else cprintf("%caddr%c 0x%x%c: page could not be allocated -- kill proc\n%c", RED, CYAN, rcr2(), RED, ocolor);
            myproc()->killed = 1;
            break;
        } 

        /* ¿Desbordamiento de pila y acceso a página de guarda? Muerte */
        if(myproc()!=0 && (err == 7 || err == 5)){      
            cprintf("%caddr%c 0x%x%c err %d: illegal memory access -- kill proc\n%c", RED, CYAN, rcr2(), RED, err, ocolor);
            myproc()->killed = 1;
            break;
        }

        /* En otro caso, reservamos páginas */
        char* mem = kalloc();
        if(mem == 0){ cprintf("%clazy alloc: could not allocate page (1)\n%c", RED, ocolor); myproc()->killed = 1; }
        else {  memset(mem, 0, PGSIZE);
                if(mappages(myproc()->pgdir, (void*)verr, PGSIZE, V2P(mem), PTE_W | PTE_U) < 0){
                  cprintf("%clazy alloc: could not allocate page (2)\n%c", RED, ocolor);
                  kfree(mem);
                  myproc()->killed = 1;
                }
        }
        break;
    }

  //PAGEBREAK: 13
  default:
    if(myproc() == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("%c", RED);
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpuid(), tf->eip, rcr2());
      panic("trap");
    }
    // In user space, assume process misbehaved.
    cprintf("%c", RED);
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x -- kill proc\n",
            myproc()->pid, myproc()->name, tf->trapno,
            tf->err, cpuid(), tf->eip, rcr2());
    cprintf("%c", ocolor);
    myproc()->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running
  // until it gets to the regular system call return.)
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit(tf->trapno+1);

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(myproc() && myproc()->state == RUNNING &&
     tf->trapno == T_IRQ0+IRQ_TIMER)
    yield();

  // Check if the process has been killed since we yielded
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit(tf->trapno+1);
}
