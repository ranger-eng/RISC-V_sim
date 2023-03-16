#include <stdio.h>
#include "shell.h"

// setup RISC-V core instruction format type masks, and types 
typedef struct {
  int opcode =  0b00000000000000000000000001111111;
  int opcode_shift = 0;
  int rd =      0b00000000000000000000111110000000;
  int rd_shift = 7;
  int funct3 =  0b00000000000000000111000000000000;
  int func3_shift = 12;
  int rs1 =     0b00000000000011111000000000000000;
  int rs1_shift = 15;
  int rs2 =     0b00000001111100000000000000000000;
  int rs2_shift = 20;
  int funct7 =  0b11111110000000000000000000000000;
  int funct7_shift = 25;
} r_type_mask_t;
r_type_mask_t r_type_mask;

typedef struct {
  int opcode;
  int rd;
  int funct3;
  int rs1;
  int rs2;
  int funct7;
} r_type_t;

typedef struct {
  int opcode =  0b00000000000000000000000001111111;
  int opcode_shift = 0;
  int rd =      0b00000000000000000000111110000000;
  int rd_shift = 7;
  int funct3 =  0b00000000000000000111000000000000;
  int funct3_shift = 12;
  int rs1 =     0b00000000000011111000000000000000;
  int rs1_shift = 15;
  int imm =     0b11111111111100000000000000000000;
  int imm_shift = 20;
} i_type_mask_t;
i_type_mask_t i_type_mask;

typedef struct {
  int opcode;
  int rd;
  int funct3;
  int rs1;
  int imm;
} i_type_t;

typedef struct {
  int opcode =  0b00000000000000000000000001111111;
  int opcode_shift = 0;
  int imm4_0 =  0b00000000000000000000111110000000;
  int imm4_0_shift = 7;
  int funct3 =  0b00000000000000000111000000000000;
  int funct3_shift = 12;
  int rs1 =     0b00000000000011111000000000000000;
  int rs1_shift = 15;
  int rs2 =     0b00000001111100000000000000000000;
  int rs2_shift = 20;
  int imm11_5 = 0b11111110000000000000000000000000;
  int imm11_5_shift = 25;
} s_type_mask_t;
s_type_mask_t s_type_mask;

typedef struct {
  int opcode;
  int imm4_0;
  int funct3;
  int rs1;
  int rs2;
  int imm11_5;
} s_type_t;


void fetch()
{
// only read
} 

void decode()
{
// decode opcode, function, and read register
}

void execute()
{
// do the rest  
}

void process_instruction()
{
  /* execute one instruction here. You should use CURRENT_STATE and modify
   * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
   * access memory. */
  fetch();
  decode();
  execute();
}
