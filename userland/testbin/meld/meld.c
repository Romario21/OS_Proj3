//meld.c
//created by Romario Estrella-Ramos

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <err.h>

//int meld(const char *path1, const char *file2, const char *file3);

int
main(int argc, char *argv[])
{
  
  const char *file1;
  const char *file2;
  const char *file3;

  int result = 0;
  
  //int fd1, fd2, fd3;
  //must create testfile01 and testfile02 in os161/root
  if(argc == 0){
    file1 = "testfile01";
    file2 = "testfile02";
  }

  else if(argc == 2){
    file1 = argv[0];
    file2 = argv[1];
  }

    else
      errx(1, "Usage: meld <filename> <filename>");

  file3 = "newfile";

  result = meld(file1, file2, file3);

  if(!result)
    printf("failed meld\n");
  else
    printf("Passed melding test.\n");
  
  return 0;
}

