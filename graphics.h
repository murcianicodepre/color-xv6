#ifndef _GRAPHICS_
#define _GRAPHICS_
#endif

/* 
   Some Codepage 437 graphic characters 
   source: https://en.wikipedia.org/wiki/Code_page_437#Character_set
   by diegojose.parragan@um.es
*/

/* Bytecolors: characters that represents 16 CGA colors, foreground and background */
/* FOREGROUND */
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

/* BACKGROUND */
#define BRED 0xc0
#define BLRED 0xc1
#define BMAGENTA 0xc2
#define BDMAGENTA 0xc3
#define BCYAN 0xc4
#define BDCYAN 0xc5
#define BBLUE 0xc6
#define BDBLUE 0xc7
#define BGREEN 0xc8
#define BDGREEN 0xc9
#define BYELLOW 0xca
#define BWHITE 0xcb
#define BGREY 0xcc
#define BLGREY 0xcd
#define BBLACK 0xce
#define BBROWN 0xcf


/*
#define BLACK 0x0
#define DBLUE 0x1
#define DGREEN 0x2
#define DCYAN 0x3
#define RED 0x4
#define DMAGENTA 0x5
#define BROWN 0x6
#define LGREY 0x7
#define GREY 0x8
#define BLUE 0x9
#define GREEN 0xa
#define CYAN 0xb
#define LRED 0xc
#define MAGENTA 0xd
#define YELLOW 0xe
#define WHITE 0xf
*/

/* Block characters */
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