#include <stdio.h>
#include <string.h>

int nflag = 0;
int main(int argc, char *argv[]){
/* echo with no arguments just prints a new line */
  if(argc<2){
      putchar('\n');
      return 0;
  }
  int i = 1;
  if(strcmp(argv[1], "-n") == 0){
      nflag = 1; i++;
  }
  for(; i < argc; i++){
      fputs(argv[i], stdout);
      if(i<argc-1) putchar(' ');
  }
  if(!nflag) putchar('\n');
  return 0;
}
