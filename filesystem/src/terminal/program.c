#include "../../lib/libduck.h"
void main(int argc, char ** argv);
char **mystrsplit(char *string, char delimiter);
int strptrlen(char ** strptr);
void *memcpy(void *dst, void *src, int count);

void main(int argc, char ** argv) {
    char * input = malloc(80);
    getc();
    char ** app_argv;
    while(1) {
		puts("\n$ ");
		gets(input);
        app_argv = mystrsplit(input, ' ');
        // __asm__("int $0x03" :: "a" (app_argv) );
        // app_argv = (char **)0x7002d9;//70038d;
        // puts(app_argv[0]);
        // app_argv = malloc(sizeof(char *) * 3);
        // app_argv[0] = malloc(sizeof(char)*5);
        // app_argv[1] = malloc(sizeof(char)*5);
        // app_argv[2] = 0;
        // memcpy(app_argv[1], "arg2", 5);
        // memcpy(app_argv[0], "arg1", 5);
        // puts("\n");
		execve(app_argv[0], strptrlen(app_argv), app_argv, 1);
	}
}

char **mystrsplit(char *string, char delimiter) {
    int length = 0, count = 0, i = 0, j = 0;
    while(*(string++)) {
        if (*string == delimiter) count++;
        length++;
    }
    string -= (length + 1);
    char **array = (char **)malloc(sizeof(char *) * (length + 1));
    char ** base = array;
    for(i = 0; i < count ; i++) {
        j = 0;
        while(string[j] != delimiter) j++;
        j++;
        *array = (char *)malloc(sizeof(char) * j);
        memcpy(*array, string, (j-1));
        (*array)[j-1] = '\0';
        string += j;
        array++;
    }
    *array = '\0';
    return base;  
}

int strptrlen(char ** strptr) {
    int len = 0;
    while(strptr[len++]);
    return len - 1;
}

void *memcpy(void *dst, void *src, int count){
	void *temp_dst = dst;
	while(count--) *(char *)dst++ = *(char *)src++;
	return temp_dst;
}