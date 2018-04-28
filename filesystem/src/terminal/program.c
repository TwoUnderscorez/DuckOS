#include "../../lib/libduck.h"
#include "../../lib/string.h"
void main(int argc, char ** argv);
char **mystrsplit(char *string, char delimiter);
int strptrlen(char ** strptr);
void setupelf(char * input);

void main(int argc, char ** argv) {
    char * input = malloc(80);
    char * path_var = malloc(90);
    char ** app_argv;
    int input_len = 0, i;
    while(1) {
		puts("\n$ ");
        memset(input, '\0', 80);
		gets(input);
        input_len = strlen(input);
        if(input_len < 2) continue;
        if(input[input_len-1] != ' ') {
            input[input_len-1] = ' ';
            input[input_len] = '\0';
        }
        app_argv = mystrsplit(input, ' ');
        if(!path_exists(app_argv[0])) {
            execve(app_argv[0], strptrlen(app_argv), app_argv, 1);
            i = 0;
            while(app_argv[i]) free((void *)(app_argv[i++]));
            free((void *)app_argv);
            continue;
        }
        memset(path_var, '\0', 90);
        strcpy(path_var, "/bin/");
        strcat(path_var, input);
        app_argv = mystrsplit(path_var, ' ');
        if(!path_exists(app_argv[0])) {
            execve(app_argv[0], strptrlen(app_argv), app_argv, 1);
            i = 0;
            while(app_argv[i]) free((void *)(app_argv[i++]));
            free((void *)app_argv);
            continue;
        }
        app_argv = mystrsplit(input, ' ');
        
        if(!strcmp(app_argv[0], "cls")) screen_clear();
        else if(!strcmp(app_argv[0], "exit")) _exit();
        else if(!strcmp(app_argv[0], "yield")) task_yield();
        else if(!strcmp(app_argv[0], "setupelf")) setupelf(input);
        else puts("command not found.\n");
	}
    while(1);
}

void setupelf(char * input) {
    char ** app_argv;
    // puts(input + strlen("setupelf")+1);
    app_argv = mystrsplit(input + strlen("setupelf")+1, ' ');
    if(!path_exists(app_argv[0])) {
        execve(app_argv[0], strptrlen(app_argv), app_argv, 0);
        puts("OK\n");
    }
    else {
        puts("File not found: ");
        puts(app_argv[0]);
        putc('\n');
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