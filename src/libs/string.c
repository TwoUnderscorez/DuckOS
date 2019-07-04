#include "string.h"
#include "../drivers/screen.h"
#include "../kernel/heap.h"

/**
 * @brief Int TO Ascii
 * 
 * @param value 
 * @param str 
 * @param base 
 * @return char* 
 */
char *itoa(int value, char *str, int base)
{
    char *rc;
    char *ptr;
    char *low;
    // Check for supported base.
    if (base < 2 || base > 36)
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if (value < 0 && base == 10)
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while (value);
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while (low < ptr)
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}

/**
 * @brief Ascii TO Int
 * 
 * @param c 
 * @return int 
 */
int atoi(char *c)
{
    int value = 0;
    int sign = 1;
    if (*c == '+' || *c == '-')
    {
        if (*c == '-')
            sign = -1;
        c++;
    }
    while (isdigit(*c))
    {
        value *= 10;
        value += (int)(*c - '0');
        c++;
    }
    return (value * sign);
}

/**
 * @brief Checks whether a char is a digit
 * 
 * @param c 
 * @return char 
 */
char isdigit(char c)
{
    return ((c >= '0') && (c <= '9'));
}

/**
 * @brief Checks whether two strings are equal
 * 
 * @param str1 
 * @param str2 
 * @return char 
 */
char strcmp(char *str1, char *str2)
{
    while (*(str1) != 0 && *(str2) != 0 && *(str1) == *(str2))
    {
        ++str1;
        ++str2;
    }
    return *str1 - *str2;
}

/**
 * @brief Compares at most the first num bytes of str1 and str2
 * 
 * @param str1 
 * @param str2 
 * @param num 
 * @return char 
 */
char strncmp(char *str1, char *str2, unsigned int num)
{
    while (*(str1) != 0 && *(str2) != 0 && *(str1) == *(str2))
    {
        ++str1;
        ++str2;
        if (num-- < 1)
            break;
    }
    return *str1 - *str2;
}

/**
 * @brief Compares at most the first num bytes of data1 and data2
 * 
 * @param data1 
 * @param data2 
 * @param num 
 * @return char 
 */
char memcmp(char *data1, char *data2, unsigned int num)
{
    while (*(data1) == *(data2))
    {
        ++data1;
        ++data2;
        if (--num < 1)
            return 0;
    }
    return 1;
}

/**
 * @brief Copies str2 to str1
 * 
 * @param str1 
 * @param str2 
 */
void strcpy(char *str1, char *str2)
{
    while (*str2 != '\0')
        *(str1++) = *(str2++);
}

/**
 * @brief Copies up to n characters from the string pointed to, by src to dest.
 * 
 * @param str1 
 * @param str2 
 * @param num 
 */
void strncpy(char *str1, char *str2, unsigned int num)
{
    while (*str2 != '\0' && num-- > 0)
        *(str1++) = *(str2++);
}

/**
 * @brief Returns length of string
 * 
 * @param str 
 * @return unsigned int 
 */
unsigned int strlen(char *str)
{
    int length = 0;
    while (*str++)
        ++length;
    return length;
}

/**
 * @brief Copies n characters from memory area str2 to memory area str1
 * 
 * @param dst 
 * @param src 
 * @param count 
 * @return void* 
 */
void *memcpy(void *dst, void *src, int count)
{
    void *temp_dst = dst;
    while (count--)
        *(char *)dst++ = *(char *)src++;
    return temp_dst;
}

/**
 * @brief Copies the character c (an unsigned char) 
 * to the first n characters of the string pointed to, 
 * by the argument str.
 * 
 * @param dst 
 * @param data 
 * @param count 
 * @return void* 
 */
void *memset(void *dst, char data, int count)
{
    void *temp_dst = dst;
    while (count--)
        *(char *)dst++ = data;
    return temp_dst;
}

/**
 * @brief Appends the string pointed to by src to 
 * the end of the string pointed to by dest.
 * 
 * @param dst 
 * @param src 
 * @return char* 
 */
char *strcat(char *dst, char *src)
{
    return strncat(dst, src, strlen(src));
}

/**
 * @brief Appends the string pointed to by src 
 * to the end of the string pointed to by dest 
 * up to n characters long
 * 
 * @param dst 
 * @param src 
 * @param n 
 * @return char* 
 */
char *strncat(char *dst, char *src, unsigned int n)
{
    memcpy((dst + strlen(dst)), src, n);
    return dst;
}

/**
 * @brief Scans str1 for the first occurrence of 
 * any of the characters that are part of str2.
 * 
 * @param str1 
 * @param str2 
 * @return unsigned int the number of characters 
 * of str1 read before this first occurrence.
 */
unsigned int strcspn(char *str1, char *str2)
{
    unsigned int n;
    char *p;
    for (n = 0; *str1; str1++, n++)
    {
        for (p = str2; *p && *p != *str1; p++)
            ;
        if (!*p)
            break;
    }
    return n;
}

/**
 * @brief finds the first occurrence of the substring
 * substr in the string str. The terminating '\0' 
 * characters are not compared.
 * 
 * @param str 
 * @param substr 
 * @return char* 
 */
char *strstr(char *str, char *substr)
{
    int len = strlen(substr);
    char *ref = substr;
    while (*str && *ref)
    {
        if (*str++ == *ref)
        {
            ref++;
        }
        if (!*ref)
        {
            return (str - len);
        }
        if (len == (ref - substr))
        {
            ref = substr;
        }
    }
    return (char *)-1;
}

/**
 * @brief split a string, like str.split in python
 * 
 * @param string 
 * @param delimiter 
 * @return char** 
 */
char **strsplit(char *string, const char delimiter)
{
    int length = 0, count = 0, i = 0, j = 0;
    while (*(string++))
    {
        if (*string == delimiter)
            count++;
        length++;
    }
    string -= (length + 1);
    char **array = (char **)malloc(sizeof(char *) * (length + 1));
    char **base = array;
    for (i = 0; i < (count + 1); i++)
    {
        j = 0;
        while (string[j] != delimiter)
            j++;
        j++;
        *array = (char *)malloc(sizeof(char) * j);
        memcpy(*array, string, (j - 1));
        (*array)[j - 1] = '\0';
        string += j;
        array++;
    }
    *array = '\0';
    return base;
}

/**
 * @brief print a hex dump
 * 
 * @param desc 
 * @param addr 
 * @param len 
 */
void hexDump(char *desc, void *addr, int len)
{
    int i;
    char buff[17];
    unsigned char *pc = (unsigned char *)addr;

    // Output description if given.
    if (desc)
        puts(desc);
    puts(":");

    if (!len)
    {
        puts("  ZERO LENGTH\n");
        return;
    }
    if (len < 0)
    {
        puts("  NEGATIVE LENGTH\n");
        return;
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++)
    {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0)
        {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
            {
                puts(buff);
                puts("\n");
            }

            // Output the offset.
            // printf ("  %04x ", i);
            screen_print_int(i, 16);
            puts(": ");
        }

        // Now the hex code for the specific character.
        screen_print_int(pc[i], 16);
        puts(" ");

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0)
    {
        puts("   ");
        i++;
    }

    // And print the final ASCII bit.
    puts("  ");
    puts(buff);
    puts("\n");
}