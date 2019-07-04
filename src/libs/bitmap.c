#include "bitmap.h"

static int get(unsigned char, unsigned char);
static void set(unsigned char *, unsigned char);
static void reset(unsigned char *, unsigned char);

/*
 copyright
 https://codereview.stackexchange.com/questions/8213/bitmap-implementation
 */

/**
 * @brief gets the value of the bit at pos 
 * 
 * @param bitmap
 * @param pos 
 * @return int 
 */
int bitmapGet(unsigned char *bitmap, int pos)
{
    return get(bitmap[pos / BIT], pos % BIT);
}

/**
 * @brief sets bit at pos to 1 
 * 
 * @param bitmap 
 * @param pos 
 */
void bitmapSet(unsigned char *bitmap, int pos)
{
    set(&bitmap[pos / BIT], pos % BIT);
}

/**
 * @brief sets bit at pos to 0
 * 
 * @param bitmap 
 * @param pos 
 */
void bitmapReset(unsigned char *bitmap, int pos)
{
    reset(&bitmap[pos / BIT], pos % BIT);
}

/**
 * @brief 
 * 
 * @param bitmap Finds the first n value in bitmap after start
 * size is the Bitmap size in unsigned chars
 * @param n 
 * @param size 
 * @param start 
 * @return int 
 */
int bitmapSearch(unsigned char *bitmap, int n, int size, int start)
{
    int i;
    /* size is now the Bitmap size in bits */
    for (i = start + 1, size *= BIT; i < size; i++)
        if (bitmapGet(bitmap, i) == n)
            return i;
    return BITMAP_NOTFOUND;
}

/**
 * @brief get a bit from a char
 * 
 * @param a 
 * @param pos 0<pos<7
 * @return int 
 */
static int get(unsigned char a, unsigned char pos)
{
    return (a >> pos) & 1;
}

/**
 * @brief set bit pos in a to 1
 * 
 * @param a 
 * @param pos 0<pos<7
 */
static void set(unsigned char *a, unsigned char pos)
{
    *a |= 1 << pos;
}

/**
 * @brief set bit pos in a to 0
 * 
 * @param a 
 * @param pos 0<pos<7
 */
static void reset(unsigned char *a, unsigned char pos)
{
    *a &= ~(1 << pos);
}