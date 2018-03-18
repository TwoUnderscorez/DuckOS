#include "bitmap.h"

static int  get  (unsigned char,   unsigned char);
static void set  (unsigned char *, unsigned char);
static void reset(unsigned char *, unsigned char);

/* CAREFUL WITH pos AND BITMAP SIZE! */

int  bitmapGet(unsigned char *bitmap, int pos) {
/* gets the value of the bit at pos */
    return get(bitmap[pos/BIT], pos%BIT);
}

void bitmapSet(unsigned char *bitmap, int pos) {
/* sets bit at pos to 1 */
    set(&bitmap[pos/BIT], pos%BIT);
}

void bitmapReset(unsigned char *bitmap, int pos) {
/* sets bit at pos to 0 */
    reset(&bitmap[pos/BIT], pos%BIT);
}

int bitmapSearch(unsigned char *bitmap, int  n, int size, int start) {
/* Finds the first n value in bitmap after start */
/* size is the Bitmap size in unsigned chars */
    int i;
    /* size is now the Bitmap size in bits */
    for(i = start+1, size *= BIT; i < size; i++)
        if(bitmapGet(bitmap,i) == n)
            return i;
    return BITMAP_NOTFOUND;
}

static int  get(unsigned char a, unsigned char pos) {
/* pos is something from 0 to 7*/
    return (a >> pos) & 1;
}

static void set(unsigned char *a, unsigned char pos) {
/* pos is something from 0 to 7*/
/* sets bit to 1 */
    *a |= 1 << pos;
}

static void reset(unsigned char *a, unsigned char pos) {
/* pos is something from 0 to 7*/
/* sets bit to 0 */
    *a &= ~(1 << pos);
}