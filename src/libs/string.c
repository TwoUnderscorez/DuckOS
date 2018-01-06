#include "string.h"

// Int TO Ascii
char * itoa( int value, char * str, int base )
{
	char * rc;
	char * ptr;
	char * low;
	// Check for supported base.
	if ( base < 2 || base > 36 )
	{
		*str = '\0';
		return str;
	}
	rc = ptr = str;
	// Set '-' for negative decimals.
	if ( value < 0 && base == 10 )
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
	} while ( value );
	// Terminating the string.
		*ptr-- = '\0';
	// Invert the numbers.
	while ( low < ptr )
	{
		char tmp = *low;
		*low++ = *ptr;
		*ptr-- = tmp;
	}
	return rc;
}

// Ascii TO Int
int atoi(char *c)
{
    int value = 0;
    int sign = 1;
    if( *c == '+' || *c == '-' )
    {
        if( *c == '-' ) sign = -1;
        c++;
    }
    while (isdigit(*c))
    {
        value *= 10;
        value += (int) (*c-'0');
        c++;
    }
    return (value * sign);
}

// Checks whether a char is a digit
char isdigit(char c){
    return ( (c>='0') && (c<='9') );
}

// Checks whether two strings are equal
char strcmp(char *str1, char *str2){
	while(*(str1) != 0 && *(str2) != 0 && *(str1) == *(str2)){
		++str1;
		++str2;
	}
	return *str1 - *str2;
}

// Compares at most the first num bytes of str1 and str2
char strncmp(char *str1, char *str2, unsigned int num) {
	while( *(str1) != 0 && *(str2) != 0 && *(str1) == *(str2) ){
		++str1;
		++str2;
		if(num-- < 1)
			break;
	}
	return *str1 - *str2;
}

// Copies str2 to str1
void strcpy(char *str1, char *str2){
	while(*str2 != '\0') *(str1++) = *(str2++);
}

// Copies up to n characters from the string pointed to, by src to dest. 
void strncpy(char *str1, char *str2, unsigned int num){
	while(*str2 != '\0' && num-- > 0) *(str1++) = *(str2++);
}

// Returns length of string
unsigned int strlen(char *str){
	int length = 0;
	while(*str++) ++length;
	return length;
}

// Copies n characters from memory area str2 to memory area str1
void *memcpy(void *dst, void *src, int count){
	void *temp_dst = dst;
	while(count--) *(char *)dst++ = *(char *)src++;
	return temp_dst;
}

// Copies the character c (an unsigned char) to the first n characters of the string pointed to, by the argument str.
void *memset(void *dst, char data, int count){
	void *temp_dst = dst;
	while(count--) *(char *)dst++ = data;
	return temp_dst;
}

// Appends the string pointed to by src to the end of the string pointed to by dest.
char *strcat(char *dst, char *src) {
	return strncat(dst, src, strlen(src));
}

// Appends the string pointed to by src to the end of the string pointed to by dest up to n characters long
char *strncat(char *dst, char *src, unsigned int n) {
	memcpy((dst+strlen(dst)), src, n);
	return dst;
}

//Scans str1 for the first occurrence of any of the characters that are part of str2, returning the number of characters of str1 read before this first occurrence.
unsigned int strcspn(char* str1, char* str2) {
	unsigned int n;
	char* p;
	for(n=0; *str1; str1++, n++) {
		for(p=str2; *p && *p != *str1; p++)
			;
		if (!*p)
			break;
	}
	return n;
}

// finds the first occurrence of the substring substr in the string str. The terminating '\0' characters are not compared.
char *strstr(char *str, char *substr)
{
    int len = strlen(substr);
    char *ref = substr;
    while(*str && *ref)
    {
        if (*str++ == *ref)
        {
            ref++;
        }
        if(!*ref)
        {
            return (str - len);
        }
        if (len == (ref - substr))
        {
            ref = substr;
        }
    }
    return '\0';
}