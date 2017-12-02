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
	  return EINVAL;
	}
	
	
	kpath = (char*)kmalloc(sizeof(char)*(PATH_MAX));

	//int temp = openfile_open(kpath, flags, mode, &file);

	result = copyinstr(upath, kpath, sizeof(kpath), &actual);//check
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


       bool check = VOP_ISSEEKABLE(file->of_node);
       if(check){
	 pos = file->of_offset;
	 lock_aquire(file->of_offsetlock);//check
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


       bool check = VOP_ISSEEKABLE(file->of_node);
       if(check){
	 pos = file->of_offset;
	 lock_aquire(file->of_offsetlock);//check
       }
       
       
       if(file->of_accmode == O_RDONLY){
	 lock_release(file->of_offsetlock);
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
  int result = 0;
  struct openfile *file;

  result = filetable_okfd(curproc->p_filetable,fd);//check
  if(result == 0)
    return EBADF;

  filetable_placeat(curproc->p_filetable, NULL, fd, &file);//check

  //check if previous entry in filetable is null


  
  openfile_decref(file);
  
  return result;

}

int
sys_meld(userptr_t pn1, userptr_t pn2, userptr_t pn3, int *retval)
{
  int fd1, fd2, fd3;
  int result = 0;
  struct openfile *file1;
  struct openfile *file2;
  struct openfile *file3;

  

  

  return result;
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
