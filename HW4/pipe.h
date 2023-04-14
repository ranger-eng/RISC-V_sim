#ifndef _PIPE_H_
#define _PIPE_H_
#define DEBUG_BOOL 1
#define DEBUG if(DEBUG_BOOL)

#include "shell.h"
#include "stdbool.h"
#include <limits.h>
#include <inttypes.h>
#include "sim_types.h"

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

/* global variable -- pipeline state */
extern CPU_State CURRENT_STATE;
extern int RUN_BIT;

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

/* definitions for pipeline registers */
typedef struct {
  bool start_ID;
  bool nop;
  uint32_t pc;
  uint32_t instruction;
} pipe_reg_IFtoID_t;

typedef struct {
  bool start_EX;
  uint32_t pc;
  uint32_t instruction;
  uint32_t rs1_value;
  uint32_t rs2_value;
  riscv_decoded_t riscv_decoded;
} pipe_reg_IDtoEX_t;

typedef struct {
  bool start_MEM;
  uint32_t pc;
  uint32_t instruction;
  int32_t alu_result;
  uint32_t rs2_value;
  riscv_decoded_t riscv_decoded;
  // control
  bool mem_branch;
  bool mem_read;
  bool mem_write;
  bool wb_regwrite;
} pipe_reg_EXtoMEM_t;

typedef struct {
  bool start_WB;
  int data;
  uint32_t pc;
  riscv_decoded_t riscv_decoded;
  // control
  bool wb_regwrite;
} pipe_reg_MEMtoWB_t;

/* function prototypes for decode */
enum instruction_format_t decode_opcode (uint32_t instruction);
riscv_decoded_t decode_r_type(uint32_t instruction);
riscv_decoded_t decode_i_type(uint32_t instruction);
riscv_decoded_t decode_s_type(uint32_t instruction);
riscv_decoded_t decode_sb_type(uint32_t instruction);
riscv_decoded_t decode_u_type(uint32_t instruction);
riscv_decoded_t decode_uj_type(uint32_t instruction);

int32_t execute_r_type(riscv_decoded_t decoded_instruction);
int32_t execute_i_type(riscv_decoded_t decoded_instruction);
int32_t execute_s_type(riscv_decoded_t decoded_instruction);
int32_t execute_sb_type(riscv_decoded_t decoded_instruction);
int32_t execute_u_type(riscv_decoded_t decoded_instruction);
int32_t execute_uj_type(riscv_decoded_t decoded_instruction);

/* functions for conditional forwarding. Pass NULL to ignore rs */
void forward(riscv_decoded_t riscv_decoded, uint32_t* rs1_value, uint32_t* rs2_value);
void forward_ex_mem(riscv_decoded_t riscv_decoded, uint32_t* rs1_value, uint32_t* rs2_value);
void forward_wb(riscv_decoded_t riscv_decoded, uint32_t* rs1_value, uint32_t* rs2_value);

#endif