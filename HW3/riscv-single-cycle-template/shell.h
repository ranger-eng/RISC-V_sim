/***************************************************************/
/*                                                             */
/*   RISC-V Instruction Level Simulator                          */
/*                                                             */
/***************************************************************/

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*          DO NOT MODIFY THIS FILE!                            */
/*          You should only change sim.c!                       */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#ifndef _SIM_SHELL_H_
#define _SIM_SHELL_H_

#include <inttypes.h>
#define FALSE 0
#define TRUE  1

#define RISCV_REGS 32

typedef struct CPU_State_Struct {
  uint32_t PC;		/* program counter */
  int32_t REGS[RISCV_REGS]; /* register file. */
  int FLAG_NV;        /* invalid operation */
  int FLAG_DZ;        /* divide by zero */
  int FLAG_OF;        /* overflow */
  int FLAG_UF;        /* underflow */
  int FLAG_NX;        /* inexact */
} CPU_State;

enum Opcode {SPECIAL, J};

/* Data Structure for Latch */

extern CPU_State CURRENT_STATE, NEXT_STATE;

extern int RUN_BIT;	/* run bit */

uint32_t mem_read_32(uint32_t address);
void     mem_write_32(uint32_t address, uint32_t value);

/* YOU IMPLEMENT THIS FUNCTION */
void process_instruction();

#endif
