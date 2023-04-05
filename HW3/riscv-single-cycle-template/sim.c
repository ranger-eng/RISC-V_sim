// Austin Mackey - Computer Architecture - HW3
// There are two headers included, sim_types.h
// which includes type def info and declarations of the bit masks
// Also, sim.h, which contains function definitions.
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
  switch (current_riscv_decoded.inst_format) {
    case R:
      execute_r_type(current_riscv_decoded);
      break;
    case I:
      execute_i_type(current_riscv_decoded);
      break;
    case S:
      execute_s_type(current_riscv_decoded);
      break;
    case SB:
      execute_sb_type(current_riscv_decoded);
      break;
    case U:
      execute_u_type(current_riscv_decoded);
      break;
    case UJ:
      execute_uj_type(current_riscv_decoded);
      break;
    default:
      printf("Invalid type\n");
      return;
  }
}

void process_instruction()
{
  /* execute one instruction here. You should use CURRENT_STATE and modify
   * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
   * access memory. */
  fetch();
  if (current_instruction == 0) {
    RUN_BIT = 0;
  }
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

// execute function definitions
void execute_r_type(riscv_decoded_t riscv_decoded) {
  int32_t rd_value;
  int32_t rs1_value;
  int32_t rs2_value;

  // Implement ADD
  if ((riscv_decoded.funct3 == 0) && (riscv_decoded.funct7 == 0)) {
    rs1_value = CURRENT_STATE.REGS[riscv_decoded.rs1];
    rs2_value = CURRENT_STATE.REGS[riscv_decoded.rs2];

    rd_value = rs1_value + rs2_value;

    NEXT_STATE.REGS[riscv_decoded.rd] = rd_value;
  }
  // Implement SLT
  else if ((riscv_decoded.funct3 == 2) && (riscv_decoded.funct7 == 0)) {
    NEXT_STATE.REGS[riscv_decoded.rd] = (CURRENT_STATE.REGS[riscv_decoded.rs1] < CURRENT_STATE.REGS[riscv_decoded.rs2]) ? 1 : 0;
  }
  else {
    printf("Instruction not recognised\n");
  }

  NEXT_STATE.PC = CURRENT_STATE.PC + 4;

}

void execute_i_type(riscv_decoded_t riscv_decoded) {
  // Implement ADDI
  if (riscv_decoded.funct3 == 0) {
    NEXT_STATE.REGS[riscv_decoded.rd] = CURRENT_STATE.REGS[riscv_decoded.rs1] + riscv_decoded.imm;
  }
  // Implement SLLI
  else if (riscv_decoded.funct3 == 1) {
    NEXT_STATE.REGS[riscv_decoded.rd] = CURRENT_STATE.REGS[riscv_decoded.rs1] << riscv_decoded.imm;
  }
  else {
    printf("Instruction not recognised\n");
  }
  NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void execute_s_type(riscv_decoded_t riscv_decoded) {
  // Implement SW
  if (riscv_decoded.funct3 == 2) {
    mem_write_32(CURRENT_STATE.REGS[riscv_decoded.rs1] + riscv_decoded.imm, CURRENT_STATE.REGS[riscv_decoded.rs2]);
  }
  else {
    printf("Instruction not recognised\n");
  }
  NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void execute_sb_type(riscv_decoded_t riscv_decoded) {
  // Implement BNE
  if (riscv_decoded.funct3 == 1) {
    if (CURRENT_STATE.REGS[riscv_decoded.rs1] != CURRENT_STATE.REGS[riscv_decoded.rs2]) {
      NEXT_STATE.PC = CURRENT_STATE.PC + riscv_decoded.imm;
    }
    else {
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
  }
  else {
    printf("Intruction not recognised\n");
  }
}

void execute_u_type(riscv_decoded_t riscv_decoded) {
  // Implement AUIPC
  NEXT_STATE.REGS[riscv_decoded.rd] = CURRENT_STATE.PC + riscv_decoded.imm;
  NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void execute_uj_type(riscv_decoded_t riscv_decoded) {
  uint32_t rd_value;

  // Implement JAL
  rd_value = CURRENT_STATE.PC + 4;

  NEXT_STATE.REGS[riscv_decoded.rd] = rd_value; 
  NEXT_STATE.PC = CURRENT_STATE.PC + riscv_decoded.imm;

}

// decode function definitions. Decoding is done using bit masks that are defined in sim_types.h
riscv_decoded_t decode_r_type(uint32_t instruction) {
  riscv_decoded_t riscv_decoded = {0};

  riscv_decoded.inst_format = R;
  riscv_decoded.opcode = instruction & opcode_mask;
  riscv_decoded.rd = (instruction & r_type_mask.rd) >> r_type_mask.rd_shift;
  riscv_decoded.funct3 = (instruction & r_type_mask.funct3) >> r_type_mask.funct3_shift;
  riscv_decoded.rs1 = (instruction & r_type_mask.rs1) >> r_type_mask.rs1_shift;
  riscv_decoded.rs2 = (instruction & r_type_mask.rs2) >> r_type_mask.rs2_shift;
  riscv_decoded.funct7 = (instruction & r_type_mask.funct7) >> r_type_mask.funct7_shift;

  DEBUG printf("PC0x%08x: 0x%08x decoded: [opcode 0x%08x] [rd 0x%08x] [funct3 0x%08x] [rs1 0x%08x] [rs2 0x%08x] [funct7 0x%08x]\n", CURRENT_STATE.PC, instruction, riscv_decoded.opcode, riscv_decoded.rd, riscv_decoded.funct3, riscv_decoded.rs1, riscv_decoded.rs2, riscv_decoded.funct7);
  return riscv_decoded;
}

riscv_decoded_t decode_i_type(uint32_t instruction) {
  riscv_decoded_t riscv_decoded = {0};

  riscv_decoded.inst_format = I;
  riscv_decoded.opcode = instruction & opcode_mask;
  riscv_decoded.rd = (instruction & i_type_mask.rd) >> i_type_mask.rd_shift;
  riscv_decoded.funct3 = (instruction & i_type_mask.funct3) >> i_type_mask.funct3_shift;
  riscv_decoded.rs1 = (instruction & i_type_mask.rs1) >> i_type_mask.rs1_shift;
  
  // handle immediate in a special way
  riscv_decoded.imm = (instruction & i_type_mask.imm) >> i_type_mask.imm_shift;

  if(riscv_decoded.imm & 0x00000800) {
    riscv_decoded.imm = riscv_decoded.imm | 0xFFFFF800;
  }

  DEBUG printf("PC0x%08x: 0x%08x decoded: [opcode 0x%08x] [rd 0x%08x] [funct3 0x%08x] [rs1 0x%08x] [imm 0x%08x]\n", CURRENT_STATE.PC, instruction, riscv_decoded.opcode, riscv_decoded.rd, riscv_decoded.funct3, riscv_decoded.rs1, riscv_decoded.imm);
  return riscv_decoded;
}

riscv_decoded_t decode_s_type(uint32_t instruction) {
  riscv_decoded_t riscv_decoded = {0};

  riscv_decoded.inst_format = S;
  riscv_decoded.opcode = instruction & opcode_mask;
  riscv_decoded.funct3 = (instruction & s_type_mask.funct3) >> s_type_mask.funct3_shift;
  riscv_decoded.rs1 = (instruction & s_type_mask.rs1) >> s_type_mask.rs1_shift;
  riscv_decoded.rs2 = (instruction & s_type_mask.rs2) >> s_type_mask.rs2_shift;
  
  // handle immediate in a special way
  uint32_t temp1 = (instruction & s_type_mask.immlow) >> s_type_mask.immlow_shift;
  uint32_t temp2 = (instruction & s_type_mask.immhigh) >> s_type_mask.immhigh_shift;
  riscv_decoded.imm = temp1 | temp2;

  DEBUG printf("PC0x%08x: 0x%08x decoded: [temp1 0x%08x] [temp2 0x%08x]\n", CURRENT_STATE.PC, instruction, temp1, temp2);

  if(riscv_decoded.imm & 0x00000800) {
    riscv_decoded.imm = riscv_decoded.imm | 0xFFFFF800;
  }

  DEBUG printf("PC0x%08x: 0x%08x decoded: [opcode 0x%08x] [imm 0x%08x] [funct3 0x%08x] [rs1 0x%08x] [rs2 0x%08x]\n", CURRENT_STATE.PC, instruction, riscv_decoded.opcode, riscv_decoded.imm, riscv_decoded.funct3, riscv_decoded.rs1, riscv_decoded.rs2);
  return riscv_decoded;
}

riscv_decoded_t decode_sb_type(uint32_t instruction) {
  riscv_decoded_t riscv_decoded = {0};

  riscv_decoded.inst_format = SB;
  riscv_decoded.opcode = instruction & opcode_mask;
  riscv_decoded.funct3 = (instruction & sb_type_mask.funct3) >> sb_type_mask.funct3_shift;
  riscv_decoded.rs1 = (instruction & sb_type_mask.rs1) >> sb_type_mask.rs1_shift;
  riscv_decoded.rs2 = (instruction & sb_type_mask.rs2) >> sb_type_mask.rs2_shift;

  // handle immediate in a special way
  uint32_t temp1 = (instruction & sb_type_mask.immlow);
  uint32_t temp2 = (instruction & sb_type_mask.immhigh);

  riscv_decoded.imm = ((temp1 >> 7) & 0xFFFFFFFE);

  if ((temp1 >> 7) & 0x0000001) {
    riscv_decoded.imm = riscv_decoded.imm | (0x00000800);
  }

  riscv_decoded.imm = riscv_decoded.imm | ((temp2 & 0x7E000000) >> 20);

  if (temp2 & 0x80000000) {
    riscv_decoded.imm = riscv_decoded.imm | 0xFFFFF000;
  }
  

  
  DEBUG printf("PC0x%08x: 0x%08x decoded: [opcode 0x%08x] [imm 0x%08x] [funct3 0x%08x] [rs1 0x%08x] [rs2 0x%08x]\n", CURRENT_STATE.PC, instruction, riscv_decoded.opcode, riscv_decoded.imm, riscv_decoded.funct3, riscv_decoded.rs1, riscv_decoded.rs2);
  return riscv_decoded;
}

riscv_decoded_t decode_u_type(uint32_t instruction) {
  riscv_decoded_t riscv_decoded = {0};

  riscv_decoded.inst_format = U;
  riscv_decoded.opcode = instruction & opcode_mask;
  riscv_decoded.rd = (instruction & u_type_mask.rd) >> u_type_mask.rd_shift;
  riscv_decoded.imm = instruction & u_type_mask.imm;

  DEBUG printf("PC0x%08x: 0x%08x decoded: [opcode 0x%08x] [rd 0x%08x] [imm 0x%08x]\n", CURRENT_STATE.PC, instruction, riscv_decoded.opcode, riscv_decoded.rd, riscv_decoded.imm);
  return riscv_decoded;
}

riscv_decoded_t decode_uj_type(uint32_t instruction) {
  riscv_decoded_t riscv_decoded = {0};
  
  riscv_decoded.inst_format = UJ;
  riscv_decoded.opcode = instruction & opcode_mask;
  riscv_decoded.rd = (instruction & uj_type_mask.rd) >> uj_type_mask.rd_shift;

  // handle immediate in a special way
  uint32_t temp1 = (instruction & uj_type_mask.imm);

  if (temp1 & 0x80000000) {
    riscv_decoded.imm = 0xFFF00000;
  }
  riscv_decoded.imm = riscv_decoded.imm | (temp1 & 0x000FF000);
  riscv_decoded.imm = riscv_decoded.imm | ((temp1 & 0x00100000) >> 8);
  riscv_decoded.imm = riscv_decoded.imm | ((temp1 & 0x7E000000) >> 19);
  riscv_decoded.imm = riscv_decoded.imm | ((temp1 & 0x01E00000) >> 19);

  DEBUG printf("PC0x%08x: 0x%08x decoded: [opcode 0x%08x] [rd 0x%08x] [imm 0x%08x]\n", CURRENT_STATE.PC, instruction, riscv_decoded.opcode, riscv_decoded.rd, riscv_decoded.imm);
  return riscv_decoded;
}