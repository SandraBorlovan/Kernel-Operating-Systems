#ifndef __LIBC_H
#define __LIBC_H

#include <stddef.h>
#include <stdint.h>

// write n bytes from x to the file descriptor fd
int write( int fd, void* x, size_t n );
 
//read n bytes from stdin
int read( int fd, void* buf, size_t nbytes );

int fork();

int execute(int idProcess);

int _exit();

int require(int idProcess);

int send( int  idProcess, uint32_t info );

int creat(char* name, int flag, int type);

#endif
