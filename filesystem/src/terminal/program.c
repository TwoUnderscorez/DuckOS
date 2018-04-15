#include "../../lib/libduck.h"
void main(void);

void main(void) {
    char * mystr = malloc(sizeof(char)*100);
    puts("> ");
    gets(mystr);
    puts(mystr);
    puts("\n");
}