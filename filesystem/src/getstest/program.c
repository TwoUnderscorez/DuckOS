#include "../../lib/libduck.h"
int main(void);

int main(void)
{
    char *mystr = malloc(sizeof(char) * 100);
    puts("> ");
    gets(mystr);
    puts(mystr);
    puts("\n");
}