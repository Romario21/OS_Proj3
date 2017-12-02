/*
 * File-related system call implementations.
 */

#include <types.h>
#include <kern/errno.h>
#include <kern/fcntl.h>
#include <kern/limits.h>
#include <kern/seek.h>
#include <kern/stat.h>
#include <lib.h>
#include <uio.h>
#include <proc.h>
#include <current.h>
#include <synch.h>
#include <copyinout.h>
#include <vfs.h>
#include <vnode.h>
#include <openfile.h>
#include <filetable.h>
#include <syscall.h>

/*
 * open() - get the path with copyinstr, then use openfile_open and
 * filetable_place to do the real work.
 */
int
sys_open(const_userptr_t upath, int flags, mode_t mode, int *retval)
{
	const int allflags = O_ACCMODE | O_CREAT | O_EXCL | O_TRUNC | O_APPEND | O_NOCTTY;

	char *kpath;
	struct openfile *file;
	int result = 0;
	size_t actual;

	/* 
	 * Your implementation of system call open starts here.  
	 *
	 * Check the design document design/filesyscall.txt for the steps
	 */

	//copy supplied path name

	if((flags & allflags) != allflags){ //check
	  //return EINVAL;
	}
	
	
	kpath = (char*)kmalloc(sizeof(char)*(PATH_MAX));

	//int temp = openfile_open(kpath, flags, mode, &file);

	result = copyinstr(upath, kpath, PATH_MAX, &actual);//check
	if(result){
	  kfree(kpath);
	  return result;
	}
	
	//openfile_open();
	result = openfile_open(kpath, flags, mode, &file);
	if(result){
	  kfree(kpath);
	  return result;
	}
	
	result = filetable_place(curproc->p_filetable, file, retval);//check
	if(result){
	  kfree(kpath);
	  return result;
	}
	
			       /*	
	(void) upath; // suppress compilation warning until code gets written
	(void) flags; // suppress compilation warning until code gets written
	(void) mode; // suppress compilation warning until code gets written
	(void) retval; // suppress compilation warning until code gets written
	(void) allflags; // suppress compilation warning until code gets written
	(void) kpath; // suppress compilation warning until code gets written
	(void) file; // suppress compilation warning until code gets written
			       */

	return result;
}

/*
 * read() - read data from a file
 */
int
sys_read(int fd, userptr_t buf, size_t size, int *retval)
{
       int result = 0;
       off_t pos = 0;
       struct openfile *file;

       /* 
        * Your implementation of system call read starts here.  
        *
        * Check the design document design/filesyscall.txt for the steps
        */

       //translate the fd # to an open file object
       result = filetable_get(curproc->p_filetable, fd, &file);
       if(result)
	 return result;


       bool check = VOP_ISSEEKABLE(file->of_vnode);
       if(check){
	 pos = file->of_offset;
	 lock_acquire(file->of_offsetlock);//check
       }
       
       
       if(file->of_accmode == O_WRONLY){
	 //lock_release(file->of_offsetlock);
	 return EBADF; //check
       }

       //contructed uio struct
       struct uio ui;
       struct iovec io;
       
       uio_kinit(&io, &ui, buf, size, pos, UIO_READ);//chekc
       ui.uio_segflg = UIO_USERSPACE;
       ui.uio_space = proc_getas();

       //call VOP_READ
       result = VOP_READ(file->of_vnode, &ui);
       if(check)
	 {
	   file->of_offset = ui.uio_offset;
	   lock_release(file->of_offsetlock);
	 }

       //check:seek position
       //file->of_offset = ui.uio_offset;

       //unlock
       //lock_release(file->of_offsetlock);


       
       //file_table_put()
       filetable_put(curproc->p_filetable, fd, file);
       if(!result)
	 *retval = size - ui.uio_resid;


       //check

       
       /*
       (void) fd; // suppress compilation warning until code gets written
       (void) buf; // suppress compilation warning until code gets written
       (void) size; // suppress compilation warning until code gets written
       (void) retval; // suppress compilation warning until code gets written
       */
       return result;
}


/* You need to add more for sys_meld, sys_write, and sys_close */

int
sys_write(int fd, userptr_t buf, size_t size, int *retval)
{
       int result = 0;
       off_t pos = 0;
       struct openfile *file;

       /* 
        * Your implementation of system call read starts here.  
        *
        * Check the design document design/filesyscall.txt for the steps
        */

       //translate the fd # to an open file object
       result = filetable_get(curproc->p_filetable, fd, &file);
       if(result)
	 return result;


       bool check = VOP_ISSEEKABLE(file->of_vnode);
       if(check){
	 pos = file->of_offset;
	 lock_acquire(file->of_offsetlock);//check
       }
       
       
       if(file->of_accmode == O_RDONLY){
	 //lock_release(file->of_offsetlock);
	 return EBADF; //check
       }

       //contructed uio struct
       struct uio ui;
       struct iovec io;
       
       uio_kinit(&io, &ui, buf, size, pos, UIO_WRITE);//chekc
       ui.uio_segflg = UIO_USERSPACE;
       ui.uio_space = proc_getas();

       //call VOP_READ
       result = VOP_WRITE(file->of_vnode, &ui);
       if(check)
	 {
	   file->of_offset = ui.uio_offset;
	   lock_release(file->of_offsetlock);
	 }

       //check:seek position
       //file->of_offset = ui.uio_offset;

       //unlock
       //lock_release(file->of_offsetlock);


       
       //file_table_put()
       filetable_put(curproc->p_filetable, fd, file);
       if(!result)
	 *retval = size - ui.uio_resid;


       //check

       
       /*
       (void) fd; // suppress compilation warning until code gets written
       (void) buf; // suppress compilation warning until code gets written
       (void) size; // suppress compilation warning until code gets written
       (void) retval; // suppress compilation warning until code gets written
       */
       return result;
}
 
int
sys_close(int fd)
{
  //int result = 0;
  struct openfile *file;

  if(!filetable_okfd(curproc->p_filetable,fd))
    return EBADF;
  //check

  filetable_placeat(curproc->p_filetable, NULL, fd, &file);//check

  //check if previous entry in filetable is null


  
  openfile_decref(file);
  
  return 0;

}

int
sys_meld(userptr_t pn1, userptr_t pn2, userptr_t pn3, int *retval)
{
  /*
  (void) pn1; // suppress compilation warning until code gets written
       (void) pn2; // suppress compilation warning until code gets written
       (void) pn3; // suppress compilation warning until code gets written
       (void) retval;
  */

  int fd1, fd2, fd3;
  struct openfile *file1;
  struct openfile *file2;
  struct openfile *file3;

  int result = 0;
  //////////////////open file 1 ////////////////

  result = sys_open(pn1, O_RDONLY, 0664, &fd1);
  if(result)
    return result;

  result = filetable_get(curproc->p_filetable, fd1, &file1);
  if(result)
    return result;

  //////////////////open file 2///////////////////

  result = sys_open(pn2, O_RDONLY, 0664, &fd2);
  if(result)
    return result;

  result = filetable_get(curproc->p_filetable, fd2, &file2);
  if(result)
    return result;

  //////////////////creat file 3//////////////////////////////

   result = sys_open(pn3, O_CREAT, 0664, &fd3);
  if(result)
    return result;

  result = filetable_get(curproc->p_filetable, fd3, &file3);
  if(result)
    return result;

  //////////////////need to read file1 file2////////////////////


  char *temp1 = kmalloc(512);
  char *temp2 = kmalloc(512);
  char *temp3 = kmalloc(1024);
  struct uio ui1, ui2, ui3;
  struct iovec io1, io2, io3;

  uio_kinit(&io1, &ui1, temp1, 512, 0, UIO_READ);
  uio_kinit(&io2, &ui2, temp2, 512, 0, UIO_READ);

  result = VOP_READ(file1->of_vnode, &ui1);
  if(result)
    return result;

  int check1 = 512 - ui1.uio_resid;
  int i;
  if(check1 < 512){
    for(i = check1; i<512; i++){
      temp1[i] = ' ';
    }
  }


  result = VOP_READ(file2->of_vnode, &ui2);
  if(result)
    return result;

  int check2 = 512 - ui2.uio_resid;
  if(check2 < 512){
    for(i = check2; i<512; i++){
      temp2[i] = ' ';
    }
  }

  ///////////////////////write to file3 4byts ////////////////////

  //fill in 8 bytes at a time;)
  for(i = 0; i*8 < 1024; i++){
    temp3[i*8] = temp1[i*4];
    temp3[i*8 +1] = temp1[i*4 +1];
    temp3[i*8 +2] = temp1[i*4 +2];
    temp3[i*8 +3] = temp1[i*4 +3];
    
    temp3[i*8 +4] = temp2[i*4 +1];
    temp3[i*8 +5] = temp2[i*4 +2];
    temp3[i*8 +6] = temp2[i*4 +3];
    temp3[i*8 +7] = temp2[i*4 +4];

  }

  uio_kinit(&io3, &ui3, temp3, 1024, 0, UIO_WRITE);
  result = VOP_WRITE(file3->of_vnode, &ui3);
  if(result)
    return result;

  ////////////////////close files ///////////////////////////////////




  
  filetable_put(curproc->p_filetable, fd3, file3);
  sys_close(fd3);

  filetable_put(curproc->p_filetable, fd2, file2);
  sys_close(fd2);

  filetable_put(curproc->p_filetable, fd1, file1);
  sys_close(fd1);
  

  *retval = 0;
  return 0;
}
/*
 * write() - write data to a file
 */

/*
 * close() - remove from the file table.
 */

/* 
* meld () - combine the content of two files word by word into a new file
*/
