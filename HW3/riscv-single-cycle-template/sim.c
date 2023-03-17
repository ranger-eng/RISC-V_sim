#include <stdio.h>
#include "shell.h"
#include "sim_types.h"

uint32_t current_instruction;

void fetch()
{
  // look at memory address of CURRENT_STATE.PC 
  current_instruction = mem_read_32(CURRENT_STATE.PC);
  printf("current instruction is: 0x%08x\n", current_instruction);
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
