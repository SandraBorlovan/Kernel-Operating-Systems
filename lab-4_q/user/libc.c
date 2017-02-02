#include "libc.h"

int write( int fd, void* x, size_t n ) {
  int r;

  asm volatile( "mov r0, %1 \n"
                "mov r1, %2 \n"
                "mov r2, %3 \n"
                "svc #1     \n"
                "mov %0, r0 \n" 
              : "=r" (r) 
              : "r" (fd), "r" (x), "r" (n) 
              : "r0", "r1", "r2" );

  return r;
} 

int writeno(uint32_t no){
  int r;

  asm volatile( "mov r0, %1 \n"
                "svc #12     \n"
                "mov %0, r0 \n" 
              : "=r" (r) 
              : "r" (no)
              : "r0");

  return r;
}

int read(int fd, void* buf, size_t nbytes){
  int r;

  asm volatile( "mov r0, %1 \n"
                "mov r1, %2 \n"
                "mov r2, %3 \n"
                "svc #2     \n"
                "mov %0, r0 \n" 
              : "=r" (r) 
              : "r" (fd), "r" (buf), "r" (nbytes) 
              : "r0", "r1", "r2" );

  return r;
}

int fork(){
  int r;

  asm volatile( "svc #3     \n"
                "mov %0, r0 \n" 
              : "=r" (r) );

  return r;
}

int _exit(){
  int r;

  asm volatile( "svc #4     \n"
                "mov %0, r0 \n" 
              : "=r" (r) );

  return r;
}

int exit(int id){
  int r;

  asm volatile( "mov r0, %1 \n"
                "svc #5     \n"
                "mov %0, r0 \n" 
              : "=r" (r) 
              : "r" (id)
              : "r0");

  return r;
}

int execute(int idProcess){
 int r;

  asm volatile( "mov r0, %1 \n"
                "svc #6     \n"
                "mov %0, r0 \n" 
              : "=r" (r) 
              : "r" (idProcess)
              : "r0");
;

  return r;

}

int require(int idProcess){
  int r;

  asm volatile( "mov r0, %1 \n"
                "svc #7     \n"
                "mov %0, r0 \n" 
              : "=r" (r) 
              : "r" (idProcess)
              : "r0");

  return r;
}

int send( int  idProcess, uint32_t info ){
  int r;

  asm volatile(  "mov r0, %1 \n"
                 "mov r1, %2 \n"
                 "svc #8     \n"
                 "mov %0, r0 \n" 
                : "=r" (r) 
                : "r" (idProcess), "r" (info)
                : "r0", "r1");

  return r;
}

int creat(char* name, int flag, int type){
  int r;

  asm volatile( "mov r0, %1 \n"
                "mov r1, %2 \n"
                "mov r2, %3 \n"
                "svc #9     \n"
                "mov %0, r0 \n" 
              : "=r" (r) 
              : "r" (name), "r" (flag), "r" (type) 
              : "r0", "r1", "r2" );

  return r;
}

int open(char name[12],int flag){
  int r;

  asm volatile(  "mov r0, %1 \n"
                 "mov r1, %2 \n"
                 "svc #10     \n"
                 "mov %0, r0 \n" 
                : "=r" (r) 
                : "r" (name[12]), "r" (flag)
                : "r0", "r1");

  return r;
}

int close(char name[12]){
  int r;

  asm volatile( "mov r0, %1 \n"
                "svc #11     \n"
                "mov %0, r0 \n" 
              : "=r" (r) 
              : "r" (name[12])
              : "r0");

  return r;

}
/*
int path(){
  int r;

  asm volatile( "svc #13     \n"
                "mov %0, r0 \n" 
              : "=r" (r) );

  return r;
}*/



