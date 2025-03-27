/*
Copyright (c) 2025 Andrew Bonner

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
/* the buffer size used when reading files */
#define BUFFER_SIZE 8192
/* for parsing argument flags */
int nflag, bflag, vflag, tflag, eflag;

/* used when counting likes (if -n or -b is specified) */
int linecount = 1;
/* function to print line count */
void printlinecount(int linecount){
  if(linecount < 10){
     printf("     %d  ", linecount);
  } else if(linecount < 100){
     printf("    %d  ", linecount);
  } else if(linecount < 1000){
     printf("   %d  ", linecount);
  } else if (linecount < 10000){
     printf("  %d  ", linecount);
  } else if (linecount < 100000){
     printf(" %d  ", linecount);
  } else{
     printf("%d  ", linecount);
  }
  return;
}
/* used to print buffer when arguments are specified */
void filterprint(char *string, int length){
    int nl = 0;
    if(nflag == 1  && linecount == 1){printlinecount(1); linecount++;}
   for(int i = 0; i < length; i++){
      switch(string[i]){
       case '\n':
           if(eflag==1){putchar('$');}
            if(bflag==1){nl = 1;}
            if(nflag == 1 && i != length-1) {
               putchar('\n');
               printlinecount(linecount);
               linecount++;
           } else{ putchar('\n'); }
           break;
        case '\t': 
           if(bflag == 1 && (nl||linecount==1)){
                printlinecount(linecount);
                linecount++;
                nl = 0;
           }
            if(tflag==1){printf("^I");}else{putchar('\t');}
            break;
        default:
           if(bflag == 1 && (nl||linecount==1)){
                printlinecount(linecount);
                linecount++;
                nl = 0;
           }
           if(vflag == 1){
               if(!isascii(string[i])){
                putchar('M'); putchar('-'); putchar(toascii(string[i]));
               }else if(iscntrl(string[i])){
                   putchar('^'); putchar(string[i] == '\177' ? '?' :
				    string[i] | 0100);
               }else{putchar(string[i]);}
           }else{putchar(string[i]);};
      }
   }
}
/* fast buffer based implementation, alternative to reading character by character */
void catfast(FILE *file) {
   int filter = 0;
   if(nflag == 1 || vflag == 1 || bflag == 1){
        filter = 1;
   }
   fseek(file, 0, SEEK_END);
   size_t filesize = ftell(file);
   rewind(file);
   int remainder = filesize % BUFFER_SIZE;
   int iterations = (filesize-remainder) / BUFFER_SIZE;
   char buffer[BUFFER_SIZE];
   if(iterations != 0){
   for(int i = 0; i < iterations; i++){
      fread(buffer, BUFFER_SIZE, 1, file);
    if(filter){
        filterprint(buffer, BUFFER_SIZE);
    } else {
      printf("%s", buffer);
    }
   }
   }
   /* if buffer isn't set to zero it will print old values */
   memset(buffer, 0, BUFFER_SIZE);
   fread(buffer, remainder, 1, file);
   if(filter){
      filterprint(buffer, remainder);
    } else {
      printf("%s", buffer);
    }
}
/* simply reads character by character, used for stdin */
void stdincat(){
  int ch;
  while ((ch = fgetc(stdin)) != EOF) {
        putchar(ch);
    }
  return;
}
int main(int argc, char *argv[])
{
    int ch;
    while ((ch = getopt(argc, argv, "bnvteAu")) != -1) {
	switch (ch) {
    /* count lines(nonempty) */
      case 'b':
	    bflag = 1;
        nflag = 0;
	    break;
	/* count all lines including empty */
	case 'n':
	    nflag =  1;
	    break;
	/* show nonprinting characters as ^x or M-x */
	case 'v':
	    vflag = 1;
	    break;
	/* -v + tabs as ^I */
	case 't':
	    tflag = vflag = 1;
	    break;
	/* -v + add $ before every new line */
	case 'e':
	    eflag = vflag = 1;
	    break;
    /* Same as -vte */
	case 'A':
	    eflag = tflag = vflag = 1;
        break;
    /* -u is ignored */
    case 'u':
        break;
    default:
        fprintf(stderr, "usage: cat -bnvteA [file ..]\n");
        return 1;
	}
    }
    /* optind is defined when getopt() is called */
    /* if no files are specified read from stdin */
    if(optind == argc){
    stdincat();
    } else{
    /* read all the files provided within the arguments */
    for(int i = optind; i < argc; i++){
    if(strcmp(argv[i], "-") == 0){
    stdincat();
    } else {
    FILE *fp = fopen(argv[i], "r");
    if (fp == NULL){
        fprintf(stderr, "file %s not found\n", argv[i]);
        return 1;
    }
    catfast(fp);
    fclose(fp);
    }
}
}
    return 0;
}
