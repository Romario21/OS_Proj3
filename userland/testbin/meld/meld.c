//meld.c
//created by Romario Estrella-Ramos

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <err.h>

int
main(int argc, char *argv[])
{
  
  const char *file1;
  const char *file2;
  const char *file3;

  int rv;
  
  //int fd1, fd2, fd3;

  if(argc == 0){
    file1 = "test1";
    file2 = "test2";
  }

  else if(argc == 2){
    file1 = argv[0];
    file2 = argv[1];
  }

  else
    errx(1, "Usage: meld <filename> <filename>");

  file3 = "newfile";

  rv = meld(file1, file2, file3);

  if(!rv)
    printf("failed test");

  else
    printf("Passed melding test.\n");
  
  return 0;
}

