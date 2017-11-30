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

	/* 
	 * Your implementation of system call open starts here.  
	 *
	 * Check the design document design/filesyscall.txt for the steps
	 */

	//copy supplied path name
	
	kpath = (char*)kmalloc((char)(PATH_MAX));

	//int temp = openfile_open(kpath, flags, mode, &file);

	result = copyinstr(upath, kpath, sizeof(kpath), NULL);
	if(result)
	  return result;
	
	//openfile_open();
	result = openfile_open(kpath, flags, mode, &file);
	if(result)
	  return result;

	result = filetable_place(proc->p_filetable, file, NULL);
	if(result)
	  return result;
	
	
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
       struct openfile *file;

       /* 
        * Your implementation of system call read starts here.  
        *
        * Check the design document design/filesyscall.txt for the steps
        */

       //translate the fd # to an open file object
       result = filetable_get(, fd, );
       if(result)
	 return result;

       lock_aquire(file->of_lock);//check

       if(file->of_accmode == O_WRONLY){
	 lock_release(file->of_lock);
	 return dasdasd; //check
       }

       //contructed uio struct
       struct uio *temp;

       //call VOP_READ
       result = VOP_READ(file->of_vnode, temp);
       if(result)
	 {
	   lock_release(file->of_lock);
	   return result;
	 }

       //check:seek position
       file->of_offset = temp->uio_offset;

       //unlock
       lock_release(file->of_lock);

       *retval = size - temp->uio_reside; //check

       //file_table_put()
       result = filetable_put(, fd, );
       if(result)
	 return result;

       
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


  return result;
}
 
int
sys_close(int fd, userptr_t buf, size_t size, int *retval)
{
  int result = 0;

  result = filetable_okfd(proc->p_filetable,fd);
  if(result == 1){

    filetable_placeat(proc->p_filetable, NULL, fd, );

    //check if previous file was also NULL

    


  }
    
  



  return result;

}

int
sys_meld(int fd1, int fd2, int fd3)
{
  int result = 0;

  


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
