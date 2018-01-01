#ifndef STRING_H
#define STRING_H

char * itoa( int value, char * str, int base );
int atoi(const char *c);
char isdigit(char * c);
char strcmp(char *str1, char *str2);
void strcpy(char *str1, char *str2);
int strlen(char *str);

#endif