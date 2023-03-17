#include <stdio.h>
#include <inttypes.h>
#include "shell.h"
#include "sim_types.h"
#include "sim.h"

uint32_t current_instruction;
riscv_decoded_t current_riscv_decoded;
enum instruction_format_t current_inst_format;

void fetch()
{
  // look at memory address of CURRENT_STATE.PC 
  current_instruction = mem_read_32(CURRENT_STATE.PC);
  DEBUG printf("PC0x%08x: current instruction is: 0x%08x\n", CURRENT_STATE.PC, current_instruction);
} 

void decode()
{
  current_inst_format = decode_opcode(current_instruction);
  if (current_inst_format == error) {
    printf("Invalid Opcode: 0x%08x\n");
    return;
  }
  DEBUG printf("PC0x%08x: instruction type %d\n", CURRENT_STATE.PC, current_inst_format);

  switch (current_inst_format) {
    case R:
      current_riscv_decoded = decode_r_type(current_instruction);
      break;
    case I:
      current_riscv_decoded = decode_i_type(current_instruction);
      break;
    case S:
      current_riscv_decoded = decode_s_type(current_instruction);
      break;
    case SB:
      current_riscv_decoded = decode_sb_type(current_instruction);
      break;
    case U:
      current_riscv_decoded = decode_u_type(current_instruction);
      break;
    case UJ:
      current_riscv_decoded = decode_uj_type(current_instruction);
      break;
    default:
      printf("Invalid type\n");
      return;
  }

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

enum instruction_format_t decode_opcode (uint32_t instruction) {
  opcode_mask_t opcode;
  enum instruction_format_t inst_format;

  opcode = instruction & opcode_mask;
  DEBUG printf("PC0x%08x: opcode is 0x%08x\n", CURRENT_STATE.PC, opcode);

  if ((opcode == opcode_I1) || (opcode == opcode_I2) || (opcode == opcode_I3) || (opcode == opcode_I4) || (opcode == opcode_I5)) {
    inst_format = I;
  }
  else if ((opcode == opcode_U1) || (opcode == opcode_U2)) {
    inst_format = U;
  }
  else if ((opcode == opcode_S1)) {
    inst_format = S;
  }
  else if ((opcode == opcode_R1) || (opcode == opcode_R2)) {
    inst_format = R;
  }
  else if ((opcode == opcode_SB1)) {
    inst_format = SB;
  }
  else if ((opcode == opcode_UJ1)) {
    inst_format = UJ;
  }
  else {
    inst_format = error;
  }

  return inst_format;
}

riscv_decoded_t decode_r_type(uint32_t instruction) {
  riscv_decoded_t riscv_decoded = {0};

  return riscv_decoded;
}

riscv_decoded_t decode_i_type(uint32_t instruction) {
  riscv_decoded_t riscv_decoded = {0};
  return riscv_decoded;
}

riscv_decoded_t decode_s_type(uint32_t instruction) {
  riscv_decoded_t riscv_decoded = {0};
  return riscv_decoded;
}

riscv_decoded_t decode_sb_type(uint32_t instruction) {
  riscv_decoded_t riscv_decoded = {0};
  return riscv_decoded;
}

riscv_decoded_t decode_u_type(uint32_t instruction) {
  riscv_decoded_t riscv_decoded = {0};

  riscv_decoded.inst_format = U;
  riscv_decoded.opcode = instruction & opcode_mask;
  riscv_decoded.imm = instruction & u_type_mask.imm;
  riscv_decoded.rd = (instruction & u_type_mask.rd) >> u_type_mask.rd_shift;

  DEBUG printf("PC0x%08x: 0x%08x decoded: [opcode 0x%08x] [rd 0x%08x] [imm 0x%08x]\n", CURRENT_STATE.PC, instruction, riscv_decoded.opcode, riscv_decoded.rd, riscv_decoded.imm);
  return riscv_decoded;
}

riscv_decoded_t decode_uj_type(uint32_t instruction) {
  riscv_decoded_t riscv_decoded = {0};
  return riscv_decoded;
}