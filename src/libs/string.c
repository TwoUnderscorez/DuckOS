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
int atoi(const char *c)
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
char isdigit(char * c){
    return ( (*c>='0') && (*c<='9') );
}

// Checks whether two strings are equal
char strcmp(char *str1, char *str2){
	while(*(str1) != 0 && *(str2) != 0 && *(str1) == *(str2)){
		++str1;
		++str2;
	}
	return *str1 - *str2;
}

// Copies str2 to str1
void strcpy(char *str1, char *str2){
	while(*str2 != '\0') *(str1++) = *(str2++);
}

// Returns length of string
int strlen(char *str){
	int length = 0;
	while(*str++) ++length;
	return length;
}
