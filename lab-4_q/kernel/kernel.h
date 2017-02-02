#ifndef __KERNEL_H
#define __KERNEL_H

#include <stddef.h>
#include <stdint.h>

#include   "GIC.h"
#include "PL011.h"
#include "SP804.h"

#include "interrupt.h"
#include "file.h"

// Include functionality from newlib, the embedded standard C library.

#include <string.h>

// Include definitions relating to the 2 user programs.

#include "P0.h"
#include "P1.h"
#include "P2.h"
#include "shell.h"

/* The kernel source code is made simpler by three type definitions:
 *
 * - a type that captures each component of an execution context (i.e.,
 *   processor state) in a compatible order wrt. the low-level handler
     preservation and restoration prologue and epilogue,
 * - a type that captures a process identifier, which is basically an
 *   integer, and
 * - a type that captures a process PCB.
 */
typedef struct{
  char nameFile[12];
  int blockAdress;
  int rwPointer;
  int flag;            // flag =1 for RDONLY,flag = 2 for WRONLY and flag = 3 for RDWR
}fd_t;

typedef struct {
  uint32_t cpsr, pc, gpr[ 13 ], sp, lr, tos;
} ctx_t;

typedef int pid_t;

typedef struct {
  pid_t pid;
  ctx_t ctx;
  uint32_t arriveTime, priority;
  fd_t fd[10];
  int noFiles;
} pcb_t;

#endif
