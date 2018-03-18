#ifndef BITMAP
#define BITMAP

#define BIT (8*sizeof(unsigned char))
#define BITMAP_NOTFOUND -1

int  bitmapGet   (unsigned char *, int);
void bitmapSet   (unsigned char *, int);
void bitmapReset (unsigned char *, int);
int  bitmapSearch(unsigned char *, int, int, int);

#endif