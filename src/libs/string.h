#ifndef STRING_H
#define STRING_H

char * itoa( int value, char * str, int base );
int atoi(char *c);
char isdigit(char c);
char strcmp(char *str1, char *str2);
char strncmp(char *str1, char *str2, unsigned int num);
void strcpy(char *str1, char *str2);
void strncpy(char *str1, char *str2, unsigned int num);
unsigned int strlen(char *str);
void *memcpy(void *dst, void *src, int count);
void *memset(void *dst, char data, int count);
char *strcat(char *dst, char *src);
char *strncat(char *dst, char *src, unsigned int n);
unsigned int strcspn(char* str1, char* str2);
char *strstr(char *str, char *substr);
char **strsplit(char *string, const char delimiter);
void hexDump (char *desc, void *addr, int len) ;
char memcmp(char *data1, char *data2, unsigned int num);

#endif