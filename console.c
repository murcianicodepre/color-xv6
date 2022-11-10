// Console input and output.
// Input is from the keyboard or serial port.
// Output is written to the screen and serial port.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "graphics.h"

static void consputc(int);

static int panicked = 0;

static struct {
  struct spinlock lock;
  int locking;
} cons;

int CURRENT_COLOR = 0x700;    // Por defecto, el color de xv6 es gris sobre fondo negro.

/* Devuelve el byte color asociado al código de color cga */
int getbytecolor(int colorcode){
  if(colorcode == 0x0){ return 0xee;}
  else if(colorcode == 0x100){ return DBLUE; }
  else if(colorcode == 0x200){ return DGREEN; }
  else if(colorcode == 0x300){ return DCYAN; }
  else if(colorcode == 0x400){ return RED; }
  else if(colorcode == 0x500){ return DMAGENTA; }
  else if(colorcode == 0x600){ return BROWN; }
  else if(colorcode == 0x700){ return LGREY; }
  else if(colorcode == 0x800){ return GREY; }
  else if(colorcode == 0x900){ return BLUE; }
  else if(colorcode == 0xa00){ return GREEN; }
  else if(colorcode == 0xb00){ return CYAN; }
  else if(colorcode == 0xc00){ return LRED; }
  else if(colorcode == 0xd00){ return MAGENTA; }
  else if(colorcode == 0xe00){ return YELLOW; }
  else { return WHITE; }
}

/* Devuelve el código de color cga asociado al byte color */
int getcolorcode(int bytecolor){
  if(bytecolor == RED){ return 0x400; }
    else if(bytecolor == LRED){ return 0xc00; }
    else if(bytecolor == MAGENTA){ return 0xd00; }
    else if(bytecolor == DMAGENTA){ return 0x500; }
    else if(bytecolor == CYAN){ return 0xb00; }
    else if(bytecolor == DCYAN){ return 0x300; }
    else if(bytecolor == BLUE){ return 0x900; }
    else if(bytecolor == DBLUE){ return 0x100; }
    else if(bytecolor == GREEN){ return 0xa00; }
    else if(bytecolor == DGREEN){ return 0x200; }
    else if(bytecolor == YELLOW){ return 0xe00; }
    else if(bytecolor == WHITE){ return 0xf00; }
    else if(bytecolor == GREY){ return 0x800; }
    else if(bytecolor == LGREY){ return 0x700; }
    else if(bytecolor == BLACK){ return 0x000; }
    else { return 0x600; }
}

static void
printint(int xx, int base, int sign)
{
  static char digits[] = "0123456789abcdef";
  char buf[16];
  int i;
  uint x;

  if(sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);

  if(sign)
    buf[i++] = '-';

  while(--i >= 0)
    consputc(buf[i]);
}
//PAGEBREAK: 50

// Print to the console. only understands %d, %x, %p, %s.
void
cprintf(char *fmt, ...)
{
  int i, c, locking;
  uint *argp;
  char *s;

  locking = cons.locking;
  if(locking)
    acquire(&cons.lock);

  if (fmt == 0)
    panic("null fmt");

  argp = (uint*)(void*)(&fmt + 1);
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
    if(c != '%'){
      consputc(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if(c == 0)
      break;
    switch(c){
    case 'd':
      printint(*argp++, 10, 1);
      break;
    case 'x':
    case 'p':
      printint(*argp++, 16, 0);
      break;
    case 's':
      if((s = (char*)*argp++) == 0)
        s = "(null)";
      for(; *s; s++)
        consputc(*s);
      break;
    case '%':
      consputc('%');
      break;
    case 'c' :
      consputc(*argp);
      break;
    default:
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c);
      break;
    }
  }

  if(locking)
    release(&cons.lock);
}

void
panic(char *s)
{
  int i;
  uint pcs[10];

  cli();
  cons.locking = 0;
  // use lapiccpunum so that we can call panic from mycpu()
  cprintf("%clapicid %d: panic: ", RED, lapicid());
  cprintf(s);
  cprintf("\n");
  getcallerpcs(&s, pcs);
  for(i=0; i<10; i++)
    cprintf(" %p", pcs[i]);
  panicked = 1; // freeze other CPU
  for(;;)
    ;
}

//PAGEBREAK: 50
#define BACKSPACE 0x100
#define CRTPORT 0x3d4       
static ushort *crt = (ushort*)P2V(0xb8000);  // CGA memory

/* Limpia la pantalla */
void clear(){
  /* Recuperamos la posición del cursor haciendo lo mismo que cgaput */
  int pos;
  outb(CRTPORT, 14);
  pos = inb(CRTPORT+1) << 8;
  outb(CRTPORT, 15);
  pos |= inb(CRTPORT+1);

  /* Limpiamos la pantalla. Borramos todo su contenido */
  pos = 0;                                          // Movemos el cursor a la parte de arriba
  memset(crt, 0, sizeof(crt[0]) * 24 * 80);         // Limpiamos la pantalla

  /* Guardamos la nueva posición del cursor */
  outb(CRTPORT, 14);
  outb(CRTPORT+1, pos>>8);
  outb(CRTPORT, 15);
  outb(CRTPORT+1, pos);
}

/* Dibuja un array de hasta 80x25 bytes por pantalla. Solo modifica el frame buffer */
void draw(int posx, int posy, int sx, int sy, char* img, char* color){
    int sigbyte = 0;
    for(int y=0; y<sy; y++){      // Para cada línea de la imagen...
        for(int x=0; x<sx; x++){
            crt[(posy+y-1)*80 + posx+x] = (img[sigbyte] & 0xff) | getcolorcode(color[sigbyte]) ;
            sigbyte++;
        }
    }
}

/* Dibuja una línea entre dos coordenadas usando un píxel concreto y de un color */
void drawline(int ax, int ay, int bx, int by, int color, int pixel){
    /* Algoritmo de Bresenham - https://es.wikipedia.org/wiki/Algoritmo_de_Bresenham#Pseudoc%C3%B3digo_del_algoritmo */
    int dx = bx-ax, dy = by-ay;      // Diferencia en cada eje
    int incrementoXi, incrementoYi;  // Incrementos en secciones inclinadas
    int incrementoXr, incrementoYr;  // Incrementos en secciones rectas
    int k, x, y, avR, av, avI;

    /* Calculamos los incrementos para las secciones inclinadas */
    if(dy>=0){ incrementoYi = 1; }
    else { dy = -dy; incrementoYi = -1; }
    if(dx>=0){ incrementoXi = 1; }
    else { dx = -dx; incrementoXi = -1; }

    /* Calculamos los incrementos para las secciones rectas */
    if(dx>=dy){ incrementoXr = incrementoXi; incrementoYr = 0; }
    else { 
      incrementoXr = 0; 
      incrementoYr = incrementoYi; 
      k = dx; dx = dy; dy = k;  
    }

    /* Inicializamos valores */
    x = ax; y = ay;
    avR = 2*dy;                 // Avance recto
    av = avR-dx;                // Avance actual
    avI = av-dx;                // Avance inclinado

    /* Bucle principal del algoritmo */
    do{
      crt[80*(y-1) + x] = (pixel & 0xff) | color;
      if(av>=0){          // Aumentan inclinados
        x+=incrementoXi;
        y+=incrementoYi;
        av+=avI;
      } else {            // Aumentan inclinados
        x+=incrementoXr;
        y+=incrementoYr;
        av+=avR;          // Aumentan rectos
      }
    } while(x!=bx);
}

/* Fuerza el cambio del color actual */
void setColor(int colorcode){
    CURRENT_COLOR = colorcode;
}

/* Devuelve el color actual */
int getColor(){
  return getbytecolor(CURRENT_COLOR);
}

static void
cgaputc(int c)
{
  int pos;
  // Cursor position: col + 80*row.
  outb(CRTPORT, 14);
  pos = inb(CRTPORT+1) << 8;
  outb(CRTPORT, 15);
  pos |= inb(CRTPORT+1);

  if(c == '\n'){ pos += 80 - pos%80; }
  else if(c == BACKSPACE){ if(pos>0) pos--; }
  /* Revisar si se cambia codificación de caracteres de colores */
  else if((c >= 0xe6 && c <= 0xef) || c == RED || c == LRED || c == 0x10 || c == 0x11 || c == 0x1e || c == 0x1f ){ CURRENT_COLOR = getcolorcode(c); }
  else crt[pos++] = (c & 0xff) | CURRENT_COLOR;

  if(pos < 0 || pos > 25*80)
    panic("pos under/overflow");

  if((pos/80) >= 24){  // Scroll up.
    memmove(crt, crt+80, sizeof(crt[0])*23*80);
    pos -= 80;
    memset(crt+pos, 0, sizeof(crt[0])*(24*80 - pos));
  }

  outb(CRTPORT, 14);
  outb(CRTPORT+1, pos>>8);
  outb(CRTPORT, 15);
  outb(CRTPORT+1, pos);
  crt[pos] = ' ' | 0x0700;
}

void
consputc(int c)
{
  if(panicked){
    cli();
    for(;;)
      ;
  }

  if(c == BACKSPACE){
    uartputc('\b'); uartputc(' '); uartputc('\b');
  } else uartputc(c);
  cgaputc(c);
}

#define INPUT_BUF 128
struct {
  char buf[INPUT_BUF];
  uint r;  // Read index
  uint w;  // Write index
  uint e;  // Edit index
} input;

#define C(x)  ((x)-'@')  // Control-x

void
consoleintr(int (*getc)(void))
{
  int c, doprocdump = 0;

  acquire(&cons.lock);
  while((c = getc()) >= 0){
    switch(c){
    case C('P'):  // Process listing.
      // procdump() locks cons.lock indirectly; invoke later
      doprocdump = 1;
      break;
    case C('U'):  // Kill line.
      while(input.e != input.w &&
            input.buf[(input.e-1) % INPUT_BUF] != '\n'){
        input.e--;
        consputc(BACKSPACE);
      }
      break;
    case C('H'): case '\x7f':  // Backspace
      if(input.e != input.w){
        input.e--;
        consputc(BACKSPACE);
      }
      break;
    default:
      if(c != 0 && input.e-input.r < INPUT_BUF){
        c = (c == '\r') ? '\n' : c;
        input.buf[input.e++ % INPUT_BUF] = c;
        consputc(c);
        if(c == '\n' || c == C('D') || input.e == input.r+INPUT_BUF){
          input.w = input.e;
          wakeup(&input.r);
        }
      }
      break;
    }
  }
  release(&cons.lock);
  if(doprocdump) {
    procdump();  // now call procdump() wo. cons.lock held
  }
}

int
consoleread(struct inode *ip, char *dst, int n)
{
  uint target;
  int c;

  iunlock(ip);
  target = n;
  acquire(&cons.lock);
  while(n > 0){
    while(input.r == input.w){
      if(myproc()->killed){
        release(&cons.lock);
        ilock(ip);
        return -1;
      }
      sleep(&input.r, &cons.lock);
    }
    c = input.buf[input.r++ % INPUT_BUF];
    if(c == C('D')){  // EOF
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        input.r--;
      }
      break;
    }
    *dst++ = c;
    --n;
    if(c == '\n')
      break;
  }
  release(&cons.lock);
  ilock(ip);

  return target - n;
}

int
consolewrite(struct inode *ip, char *buf, int n)
{
  int i;

  iunlock(ip);
  acquire(&cons.lock);
  for(i = 0; i < n; i++)
    consputc(buf[i] & 0xff);
  release(&cons.lock);
  ilock(ip);

  return n;
}

void
consoleinit(void)
{
  initlock(&cons.lock, "console");
  devsw[CONSOLE].write = consolewrite;
  devsw[CONSOLE].read = consoleread;
  cons.locking = 1;

  ioapicenable(IRQ_KBD, 0);
}

