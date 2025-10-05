#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

#include "shell.h"

char* getpasswd(char* msg, int length) {
  char* buffer;
  struct termios term;
  
  printf("%s", msg);
  
  // Turn off echo
  tcgetattr(fileno(stdin), &term);
  term.c_lflag &= ~ECHO;
  tcsetattr(fileno(stdin), 0, &term);
  
  buffer = (char*) malloc(length);
  fgets(buffer, length, stdin);
  
  // Turn on echo
  term.c_lflag |= ECHO;
  tcsetattr(fileno(stdin), 0, &term);
  
  printf("\n");
  
  return buffer;
}
