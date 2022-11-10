#ifndef _GRAPHICS_
#define _GRAPHICS_
#endif

/* 
   Some Codepage 437 graphic characters 
   source: https://en.wikipedia.org/wiki/Code_page_437#Character_set
   by diegojose.parragan@um.es
*/

/* Bytecolors, 16 CGA colors. Uses line 0xe, which are greek letters */
#define RED 0xe0
#define LRED 0xe1
#define MAGENTA 0x10
#define DMAGENTA 0x11
#define CYAN 0x1e
#define DCYAN 0x1f
#define BLUE 0xe6
#define DBLUE 0xe7
#define GREEN 0xe8
#define DGREEN 0xe9
#define YELLOW 0xea
#define WHITE 0xeb
#define GREY 0xec
#define LGREY 0xed
#define BLACK 0xee
#define BROWN 0xef
/*
#define BLACK 0x0
#define DBLUE 0x100
#define DGREEN 0x200
#define DCYAN 0x300
#define RED 0X400
#define DMAGENTA 0x500
#define BROWN 0x600
#define LGREY 0x700
#define GREY 0x800
#define BLUE 0x900
#define GREEN 0xA00
#define CYAN 0xB00
#define LRED 0xC00
#define MAGENTA 0xD00
#define YELLOW 0xE00
#define WHITE 0xF00
*/


/* Blocks */
#define BLOCK 0xdb     // Whole block
#define HBLOCK 0xdc     // Half block, lower part
#define LBLOCK 0xdd     // Left side block
#define RBLOCK 0xde     // Right side block
#define UBLOCK 0xdf     // Half block, upper part
#define TBLOCK1 0xb0    // Transparent block 1
#define TBLOCK2 0xb1    // Transparent block 2
#define TBLOCK3 0xb2    // Transparent block 3
#define MBLOCK 0xfe     // Mini block
#define CBLOCK 0x16     // Center block

/* Lines */
#define LLEFT 0xb3      // Left line