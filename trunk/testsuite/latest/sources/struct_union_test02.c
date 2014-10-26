/*
 * struct-test.c
 *
 *  Created on: 28.07.2010
 *      Author: Alexander
 */

void ab (void) {
	while(1) {
	}
}

int main (void)
{
#ifdef __gcc__
  struct { int; int q; } a; // does not declare anything
#endif
  struct { union {int x;}; int q; } b;
  struct { struct {int x;}; int q; } c;
  union { union {int x;}; int q; } d;
  union { struct {int x;}; int q; } e;

  struct { union {int xX; int yX;}; int qX; } bX;
  union { struct {int xX;}; int qX; } eX;

    bX.yX = 10;
    bX.xX = 15;
    if (bX.yX != 15)
      ab();

    eX.xX = 10;
    eX.qX = 15;
    if (eX.xX != 15)
      ab();
    return 0;
}
