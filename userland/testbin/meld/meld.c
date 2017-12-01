//meld.c
//created by Romario Estrella-Ramos

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <err.h>

int
main(int argc, char *argv[])
{
  static char writebuf1[12] = "AAAABBBBCCCCC";
  static char readbuf1[13];

  static char writebuf2[12] = "ddddeeeeffff";
  static char readbuf2[13];
  
  const char *file1;
  const char *file2;
  const char *file3;
  
  int fd1, fd2, fd3;

  if(argc == 0){
    /*warnx("No arguments - running on \"test\"");*/
    file = "testfile";
  }

  else if(argc == 2){
    file = argv[1];
  }

  else{
    errx(1, "Usage: filetest <filename>");
  }

  	fd = open(file1, O_WRONLY|O_CREAT|O_TRUNC, 0664);
	if (fd<0) {
		err(1, "%s: open for write", file);
	}


	rv = write(fd, writebuf, 40);
	if (rv<0) {
		err(1, "%s: write", file);
	}

	rv = close(fd);
	if (rv<0) {
		err(1, "%s: close (1st time)", file);
	}

	fd = open(file, O_RDONLY);
	if (fd<0) {
		err(1, "%s: open for read", file);
	}

	rv = read(fd, readbuf, 40);
	if (rv<0) {
		err(1, "%s: read", file);
	}

	rv = close(fd);
	printf("Closing fd=%d retval=%d.\n", fd, rv);
	if (rv<0) {
		err(1, "%s: close (2nd time)", file);
	}
	/* ensure null termination */
	readbuf[40] = 0;

	if (strcmp(readbuf, writebuf)) {
		errx(1, "Buffer data mismatch!");
	}

  

  
  printf("Passed melding test.\n");
  return 0;
}

