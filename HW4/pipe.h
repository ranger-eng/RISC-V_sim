#ifndef _PIPE_H_
#define _PIPE_H_

#include "shell.h"
#include "stdbool.h"
#include <limits.h>

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

int RUN_BIT;

/* global variable -- pipeline state */
extern CPU_State CURRENT_STATE;

/* called during simulator startup */
void pipe_init();

/* this function calls the others */
void pipe_cycle();

/* each of these functions implements one stage of the pipeline */
void pipe_stage_fetch();
void pipe_stage_decode();
void pipe_stage_execute();
void pipe_stage_mem();
void pipe_stage_wb();

#endif
