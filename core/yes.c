/* repeatedly output a string
 * usage: yes [args...] */

#include <stdio.h>
int main(int argc, char *argv[]){
    /* if no arguments are provided put y */
   if(argc == 1) for(;;) { puts("y"); };
   /* otherwise print arguments */
   for(;;) { for(int i = 1; i < argc; i++){
    if(i>1) putchar(' ');
    printf("%s", argv[i]);
   } putchar('\n');}
}
