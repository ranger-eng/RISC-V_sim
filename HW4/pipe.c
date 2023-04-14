// RISCV pipeline timing simulator

#include "pipe.h"
#include "shell.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

pipe_reg_IFtoID_t pipe_reg_IFtoID = {0};
pipe_reg_IDtoEX_t pipe_reg_IDtoEX = {0};
pipe_reg_EXtoMEM_t pipe_reg_EXtoMEM = {0};
pipe_reg_MEMtoWB_t pipe_reg_MEMtoWB = {0};
// Needed to preserve WB for forwarding in exec,
// otherwise it is overwritten by mem stage before exec.
pipe_reg_MEMtoWB_t pipe_reg_WB = {0};

void pipe_init()
{
  memset(&CURRENT_STATE, 0, sizeof(CPU_State));
  CURRENT_STATE.PC = 0x00000000;
}

void pipe_cycle()
{
  pipe_stage_wb();
  pipe_stage_mem();
  pipe_stage_execute();
  pipe_stage_decode();
  pipe_stage_fetch();
}

void pipe_stage_wb()
{
  if (pipe_reg_MEMtoWB.start_WB) {

    DEBUG printf("WB; PC0x%08x: instruction type %d\n", pipe_reg_MEMtoWB.pc, pipe_reg_MEMtoWB.riscv_decoded.inst_format);

    if (pipe_reg_MEMtoWB.wb_regwrite) {
      DEBUG printf("WB; PC0x%08x: wrote %d to REG[%d]\n", pipe_reg_MEMtoWB.pc, pipe_reg_MEMtoWB.data, pipe_reg_MEMtoWB.riscv_decoded.rd);
      CURRENT_STATE.REGS[pipe_reg_MEMtoWB.riscv_decoded.rd] = pipe_reg_MEMtoWB.data;
    }

    pipe_reg_WB = pipe_reg_MEMtoWB;

    if (pipe_reg_MEMtoWB.riscv_decoded.inst_format == hlt) {
      RUN_BIT = 0;
    }
  }
}

void pipe_stage_mem()
{
  if (pipe_reg_EXtoMEM.start_MEM) {
    pipe_reg_MEMtoWB.start_WB = true;
    DEBUG printf("MEM; PC0x%08x: instruction type %d\n", pipe_reg_EXtoMEM.pc, pipe_reg_EXtoMEM.riscv_decoded.inst_format);

    // update data to be passed up to wb based on control logic
    int data;
    data = pipe_reg_EXtoMEM.alu_result;
    // Perform memory operations
    if (pipe_reg_EXtoMEM.mem_read) {
      data = mem_read_32(pipe_reg_EXtoMEM.alu_result);
    }
    if (pipe_reg_EXtoMEM.mem_write) {
      // Read directly from register file, not from pipe_reg_EXtoMEM, because it may have been updated by WB stage.
      uint32_t rs2_value = pipe_reg_EXtoMEM.rs2_value;
      // Forward from WB stage if WB rd is the same as MEM rs2
      if (pipe_reg_MEMtoWB.wb_regwrite && pipe_reg_MEMtoWB.riscv_decoded.rd == pipe_reg_EXtoMEM.riscv_decoded.rs2) {
        rs2_value = pipe_reg_MEMtoWB.data;
      }
      mem_write_32(pipe_reg_EXtoMEM.alu_result, rs2_value);
    }
    // Perform branching
    if (pipe_reg_EXtoMEM.mem_branch) {
      CURRENT_STATE.PC = data;
      // TODO: flush pipeline
    }

    // update pipe_reg_MEMtoWB
    pipe_reg_MEMtoWB.riscv_decoded = pipe_reg_EXtoMEM.riscv_decoded;
    pipe_reg_MEMtoWB.data = data;
    //control
    pipe_reg_MEMtoWB.wb_regwrite = pipe_reg_EXtoMEM.wb_regwrite;
  }
}

void pipe_stage_execute()
{
  if (pipe_reg_IDtoEX.start_EX) {
    int32_t alu_result;
    
    // start MEM
    pipe_reg_EXtoMEM.start_MEM = true;

    // reset control signals, and do the control logic in the execution functions based on the instruction
    pipe_reg_EXtoMEM.mem_branch = false;
    pipe_reg_EXtoMEM.mem_read = false;
    pipe_reg_EXtoMEM.mem_write = false;
    pipe_reg_EXtoMEM.wb_regwrite = false;

    switch (pipe_reg_IDtoEX.riscv_decoded.inst_format) {
      case R:
        alu_result = execute_r_type(pipe_reg_IDtoEX.riscv_decoded);
        break;
      case I:
        alu_result = execute_i_type(pipe_reg_IDtoEX.riscv_decoded);
        break;
      case S:
        alu_result = execute_s_type(pipe_reg_IDtoEX.riscv_decoded);
        break;
      case SB:
        alu_result = execute_sb_type(pipe_reg_IDtoEX.riscv_decoded);
        break;
      case U:
        alu_result = execute_u_type(pipe_reg_IDtoEX.riscv_decoded);
        break;
      case nop:
        // pass through
        pipe_reg_EXtoMEM.riscv_decoded.inst_format = nop;
        return;
      default:
        // EOF, do mothing.
        pipe_reg_IDtoEX.start_EX = false;
        pipe_reg_EXtoMEM.riscv_decoded = pipe_reg_IDtoEX.riscv_decoded;
        return;
    }


    DEBUG printf("EXECUTE; PC0x%08x: instruction type %d\n", pipe_reg_IDtoEX.pc, pipe_reg_IDtoEX.riscv_decoded.inst_format);
    DEBUG printf("EXECUTE; PC0x%08x: alu_result %d\n", pipe_reg_IDtoEX.pc, alu_result);

    // update pipe_reg_EXtoMEM
    pipe_reg_EXtoMEM.alu_result = alu_result;
    // handle updating the pc within the execution functions.
    // add 4 normally, or update pc based on instruciton
    // pipe_reg_EXtoMEM.pc;
    pipe_reg_EXtoMEM.instruction = pipe_reg_IDtoEX.instruction;
    pipe_reg_EXtoMEM.riscv_decoded = pipe_reg_IDtoEX.riscv_decoded;
  }
}

void pipe_stage_decode()
{
  if (pipe_reg_IFtoID.start_ID) {
    // start EX
    pipe_reg_IDtoEX.start_EX = true;

    riscv_decoded_t decoded_instruction = {0};
    enum instruction_format_t current_inst_format = decode_opcode(pipe_reg_IFtoID.instruction);

    DEBUG printf("DECODE; PC0x%08x: instruction type %d\n", pipe_reg_IFtoID.pc, current_inst_format);

    switch (current_inst_format) {
      case R:
        decoded_instruction = decode_r_type(pipe_reg_IFtoID.instruction);
        break;
      case I:
        decoded_instruction = decode_i_type(pipe_reg_IFtoID.instruction);
        break;
      case S:
        decoded_instruction = decode_s_type(pipe_reg_IFtoID.instruction);
        break;
      case SB:
        decoded_instruction = decode_sb_type(pipe_reg_IFtoID.instruction);
        break;
      case U:
        decoded_instruction = decode_u_type(pipe_reg_IFtoID.instruction);
        break;
      default:
        decoded_instruction.inst_format = hlt;
        pipe_reg_IFtoID.start_ID = false;
        return;
    }

    // If trying to write to x0, replace with hlt since x0 is read only.
    if(current_inst_format != S && current_inst_format != SB && decoded_instruction.rd == 0) {
      decoded_instruction.inst_format = hlt;
      pipe_reg_IFtoID.start_ID = false;
    }

    // Check for load-use hazards
    bool load_use_hazard = pipe_reg_EXtoMEM.mem_read && (
      pipe_reg_EXtoMEM.riscv_decoded.rd == decoded_instruction.rs1 ||
      pipe_reg_EXtoMEM.riscv_decoded.rd == decoded_instruction.rs2
    );
    if (load_use_hazard) {
      // insert nop
      pipe_reg_IDtoEX.riscv_decoded.inst_format = nop;
    } else {
      // populate pipe_reg_IDtoEX
      pipe_reg_IDtoEX.pc = pipe_reg_IFtoID.pc;
      pipe_reg_IDtoEX.rs1_value = CURRENT_STATE.REGS[decoded_instruction.rs1];
      pipe_reg_IDtoEX.rs2_value = CURRENT_STATE.REGS[decoded_instruction.rs2];
      pipe_reg_IDtoEX.instruction = pipe_reg_IFtoID.instruction;
      pipe_reg_IDtoEX.riscv_decoded = decoded_instruction;
    }
  }
}

void pipe_stage_fetch()
{
  pipe_reg_IFtoID.pc = CURRENT_STATE.PC;
  pipe_reg_IFtoID.instruction = mem_read_32(pipe_reg_IFtoID.pc);

  DEBUG printf("FETCH; PC0x%08x: instruction is 0x%08x\n", pipe_reg_IFtoID.pc, pipe_reg_IFtoID.instruction);

  // start ID
  pipe_reg_IFtoID.start_ID = true;

  // update PC
  CURRENT_STATE.PC = CURRENT_STATE.PC + 4;
}

/*----------------------------------------------------------------*/
// Functions and variables for executing
/*----------------------------------------------------------------*/

int32_t execute_r_type(riscv_decoded_t riscv_decoded) {
  int32_t alu_result;

  uint32_t rs1_value = pipe_reg_IDtoEX.rs1_value;
  uint32_t rs2_value = pipe_reg_IDtoEX.rs2_value;
  forward(riscv_decoded, &rs1_value, &rs2_value);

  // Implement ADD
  if ((riscv_decoded.funct3 == 0) && (riscv_decoded.funct7 == 0)) {
    alu_result = rs1_value + rs2_value;
    
    //control
    pipe_reg_EXtoMEM.wb_regwrite = true;
    DEBUG printf("EXECUTE; PC0x%08x: ADD %d + %d, to REG[%d]\n", pipe_reg_IDtoEX.pc, rs1_value, rs2_value, riscv_decoded.rd);
  }
  // Implement SUB
  if ((riscv_decoded.funct3 == 0) && (riscv_decoded.funct7 == 32)) {
    alu_result = rs1_value - rs2_value;
    
    //control
    pipe_reg_EXtoMEM.wb_regwrite = true;
    DEBUG printf("EXECUTE; PC0x%08x: SUB %d - %d, to REG[%d]\n", pipe_reg_IDtoEX.pc, rs1_value, rs2_value, riscv_decoded.rd);
  }

  pipe_reg_EXtoMEM.rs2_value = rs2_value;

  return alu_result;
}

int32_t execute_i_type(riscv_decoded_t riscv_decoded) {
  int32_t alu_result;

  uint32_t rs1_value = pipe_reg_IDtoEX.rs1_value;
  forward(riscv_decoded, &rs1_value, NULL);
  
  // Implement ADDI
  if (riscv_decoded.funct3 == 0) {
    alu_result = rs1_value + riscv_decoded.imm;

    // control
    pipe_reg_EXtoMEM.wb_regwrite = true;
    DEBUG printf("EXECUTE; PC0x%08x: ADDI %d + %d, to REG[%d]\n", pipe_reg_IDtoEX.pc, rs1_value, riscv_decoded.imm, riscv_decoded.rd);
  }
  // Implement LW
  if (riscv_decoded.funct3 == 2) {
    alu_result = rs1_value + riscv_decoded.imm;
    DEBUG printf("EXECUTE; PC0x%08x: LW rd:%08x, addr:%d\n", pipe_reg_IDtoEX.pc, riscv_decoded.rd, alu_result);

    // control
    pipe_reg_EXtoMEM.mem_read = true;
    pipe_reg_EXtoMEM.wb_regwrite = true;
  }

  return alu_result;
}

int32_t execute_s_type(riscv_decoded_t riscv_decoded) {
  int32_t alu_result;
  
  uint32_t rs1_value = pipe_reg_IDtoEX.rs1_value;
  uint32_t rs2_value = pipe_reg_IDtoEX.rs2_value;
  forward(riscv_decoded, &rs1_value, &rs2_value);

  // Implement SW
  if (riscv_decoded.funct3 == 2) {
    alu_result = rs1_value + riscv_decoded.imm;
    DEBUG printf("EXECUTE; PC0x%08x: SW addr:%08x, value:%d\n", pipe_reg_IDtoEX.pc, alu_result, rs2_value);

    // control
    pipe_reg_EXtoMEM.mem_write = 1;
  }

  pipe_reg_EXtoMEM.rs2_value = rs2_value;

  return alu_result;
}

int32_t execute_sb_type(riscv_decoded_t riscv_decoded) {
  int32_t alu_result;

  uint32_t rs1_value = pipe_reg_IDtoEX.rs1_value;
  uint32_t rs2_value = pipe_reg_IDtoEX.rs2_value;
  forward(riscv_decoded, &rs1_value, &rs2_value);

  // Implement blt
  if (riscv_decoded.funct3 == 4) {
    if (rs1_value < rs2_value) {
      pipe_reg_EXtoMEM.mem_branch = 1;
      alu_result = pipe_reg_EXtoMEM.pc + riscv_decoded.imm;
    }
  }

  pipe_reg_EXtoMEM.rs2_value = rs2_value;

  return alu_result;
}

int32_t execute_u_type(riscv_decoded_t riscv_decoded) {
  int32_t alu_result;

  // Implement AUIPC
  alu_result = pipe_reg_IDtoEX.pc + riscv_decoded.imm;
  DEBUG printf("EXECUTE; PC0x%08x: AUIPC rd:%d, pc+imm:%08x\n", pipe_reg_IDtoEX.pc, riscv_decoded.rd, alu_result);

  // control
  pipe_reg_EXtoMEM.wb_regwrite = true;

  return alu_result;
}

/*----------------------------------------------------------------*/
// Functions and variables for decoding
// Decoding is done using bit masks that are defined in sim_types.h and below
/*----------------------------------------------------------------*/

opcode_mask_t opcode_mask = 0x0000007f;
// define the various opcode possibilities
opcode_mask_t opcode_I1 =   0x00000003;
opcode_mask_t opcode_I2 =   0x0000000f;
opcode_mask_t opcode_I3 =   0x00000013;
opcode_mask_t opcode_I4 =   0x0000001b;
opcode_mask_t opcode_I5 =   0x00000073;
opcode_mask_t opcode_U1 =   0x00000017;
opcode_mask_t opcode_U2 =   0x00000037;
opcode_mask_t opcode_S1 =   0x00000023;
opcode_mask_t opcode_R1 =   0x00000033;
opcode_mask_t opcode_R2 =   0x0000003b;
opcode_mask_t opcode_SB1 =  0x00000063;
opcode_mask_t opcode_UJ1 =  0x0000006f;

r_type_mask_t r_type_mask = {
  .rd =      0x00000f80,
  .rd_shift = 7,
  .funct3 =  0x00007000,
  .funct3_shift = 12,
  .rs1 =     0x000f8000,
  .rs1_shift = 15,
  .rs2 =     0x01f00000,
  .rs2_shift = 20,
  .funct7 =  0xfe000000,
  .funct7_shift = 25,
};

i_type_mask_t i_type_mask = {
  .rd =      0x00000f80,
  .rd_shift = 7,
  .funct3 =  0x00007000,
  .funct3_shift = 12,
  .rs1 =     0x000f8000,
  .rs1_shift = 15,
  .imm =     0xfff00000,
  .imm_shift = 20,
};

const s_type_mask_t s_type_mask = {
  .immlow =   0x00000f80,
  .immlow_shift = 7,
  .funct3 =  0x00007000,
  .funct3_shift = 12,
  .rs1 =     0x000f8000,
  .rs1_shift = 15,
  .rs2 =     0x01f00000,
  .rs2_shift = 20,
  .immhigh = 0xfe000000,
  .immhigh_shift = 20,
};

sb_type_mask_t sb_type_mask = {
  .immlow =  0x00000f80,
  .funct3 =    0x00007000,
  .funct3_shift = 12,
  .rs1 =       0x000f8000,
  .rs1_shift = 15,
  .rs2 =       0x01f00000,
  .rs2_shift = 20,
  .immhigh = 0xfe000000,
};

u_type_mask_t u_type_mask = {
  .rd =      0x00000f80,
  .rd_shift = 7,
  .imm =     0xfffff000,
};

uj_type_mask_t uj_type_mask = {
  .rd =     0x00000f80,
  .rd_shift = 7,
  .imm =    0xfffff000,
};

enum instruction_format_t decode_opcode (uint32_t instruction) {
  opcode_mask_t opcode;
  enum instruction_format_t inst_format;

  opcode = instruction & opcode_mask;
  DEBUG printf("DECODE; PC0x%08x: opcode is 0x%08x\n", pipe_reg_IFtoID.pc, opcode);

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
    inst_format = hlt;
  }

  return inst_format;
}

riscv_decoded_t decode_r_type(uint32_t instruction) {
  riscv_decoded_t riscv_decoded = {0};

  riscv_decoded.inst_format = R;
  riscv_decoded.opcode = instruction & opcode_mask;
  riscv_decoded.rd = (instruction & r_type_mask.rd) >> r_type_mask.rd_shift;
  riscv_decoded.funct3 = (instruction & r_type_mask.funct3) >> r_type_mask.funct3_shift;
  riscv_decoded.rs1 = (instruction & r_type_mask.rs1) >> r_type_mask.rs1_shift;
  riscv_decoded.rs2 = (instruction & r_type_mask.rs2) >> r_type_mask.rs2_shift;
  riscv_decoded.funct7 = (instruction & r_type_mask.funct7) >> r_type_mask.funct7_shift;

  DEBUG printf("DECODE; PC0x%08x: 0x%08x decoded: [opcode 0x%08x] [rd 0x%08x] [funct3 0x%08x] [rs1 0x%08x] [rs2 0x%08x] [funct7 0x%08x]\n", pipe_reg_IFtoID.pc, instruction, riscv_decoded.opcode, riscv_decoded.rd, riscv_decoded.funct3, riscv_decoded.rs1, riscv_decoded.rs2, riscv_decoded.funct7);
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

  DEBUG printf("DECODE; PC0x%08x: 0x%08x decoded: [opcode 0x%08x] [rd 0x%08x] [funct3 0x%08x] [rs1 0x%08x] [imm 0x%08x]\n", pipe_reg_IFtoID.pc, instruction, riscv_decoded.opcode, riscv_decoded.rd, riscv_decoded.funct3, riscv_decoded.rs1, riscv_decoded.imm);
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

  DEBUG printf("DECODE; PC0x%08x: 0x%08x decoded: [temp1 0x%08x] [temp2 0x%08x]\n", pipe_reg_IFtoID.pc, instruction, temp1, temp2);

  if(riscv_decoded.imm & 0x00000800) {
    riscv_decoded.imm = riscv_decoded.imm | 0xFFFFF800;
  }

  DEBUG printf("DECODE; PC0x%08x: 0x%08x decoded: [opcode 0x%08x] [imm 0x%08x] [funct3 0x%08x] [rs1 0x%08x] [rs2 0x%08x]\n", pipe_reg_IFtoID.pc, instruction, riscv_decoded.opcode, riscv_decoded.imm, riscv_decoded.funct3, riscv_decoded.rs1, riscv_decoded.rs2);
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
  
  DEBUG printf("DECODE; PC0x%08x: 0x%08x decoded: [opcode 0x%08x] [imm 0x%08x] [funct3 0x%08x] [rs1 0x%08x] [rs2 0x%08x]\n", pipe_reg_IFtoID.pc, instruction, riscv_decoded.opcode, riscv_decoded.imm, riscv_decoded.funct3, riscv_decoded.rs1, riscv_decoded.rs2);
  return riscv_decoded;
}

riscv_decoded_t decode_u_type(uint32_t instruction) {
  riscv_decoded_t riscv_decoded = {0};

  riscv_decoded.inst_format = U;
  riscv_decoded.opcode = instruction & opcode_mask;
  riscv_decoded.rd = (instruction & u_type_mask.rd) >> u_type_mask.rd_shift;
  riscv_decoded.imm = instruction & u_type_mask.imm;

  DEBUG printf("DECODE; PC0x%08x: 0x%08x decoded: [opcode 0x%08x] [rd 0x%08x] [imm 0x%08x]\n", pipe_reg_IFtoID.pc, instruction, riscv_decoded.opcode, riscv_decoded.rd, riscv_decoded.imm);
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

  DEBUG printf("DECODE; PC0x%08x: 0x%08x decoded: [opcode 0x%08x] [rd 0x%08x] [imm 0x%08x]\n", pipe_reg_IFtoID.pc, instruction, riscv_decoded.opcode, riscv_decoded.rd, riscv_decoded.imm);
  return riscv_decoded;
}

void forward(riscv_decoded_t riscv_decoded, uint32_t* rs1_value, uint32_t* rs2_value) {
  // By calling ex_mem AFTER mem_wb, we prioritize forwarding ex_mem without an additional check.
  // Technically wastefull, but performance isn't critical here.
  forward_wb(riscv_decoded, rs1_value, rs2_value);
  forward_ex_mem(riscv_decoded, rs1_value, rs2_value);
}

void forward_ex_mem(riscv_decoded_t riscv_decoded, uint32_t* rs1_value, uint32_t* rs2_value) {
  if(rs1_value) {
    bool forward_rs1 = pipe_reg_MEMtoWB.wb_regwrite &&
      pipe_reg_MEMtoWB.riscv_decoded.rd == riscv_decoded.rs1 &&
      pipe_reg_MEMtoWB.riscv_decoded.rd != 0;
    *rs1_value = forward_rs1 ? pipe_reg_MEMtoWB.data : *rs1_value;
  }
  if(rs2_value) {
    bool forward_rs2 = pipe_reg_MEMtoWB.wb_regwrite &&
      pipe_reg_MEMtoWB.riscv_decoded.rd == riscv_decoded.rs2 &&
      pipe_reg_MEMtoWB.riscv_decoded.rd != 0;
    *rs2_value = forward_rs2 ? pipe_reg_MEMtoWB.data : *rs2_value;
  }
}

void forward_wb(riscv_decoded_t riscv_decoded, uint32_t* rs1_value, uint32_t* rs2_value) {  
  if(rs1_value) {
    bool forward_rs1 = pipe_reg_WB.wb_regwrite &&
      pipe_reg_WB.riscv_decoded.rd == riscv_decoded.rs1 &&
      pipe_reg_WB.riscv_decoded.rd != 0;
    *rs1_value = forward_rs1 ? pipe_reg_WB.data : *rs1_value;
  }
  if(rs2_value) {
    bool forward_rs2 = pipe_reg_WB.wb_regwrite &&
      pipe_reg_WB.riscv_decoded.rd == riscv_decoded.rs2 &&
      pipe_reg_WB.riscv_decoded.rd != 0;

    *rs2_value = forward_rs2 ? pipe_reg_WB.data : *rs2_value;
  }
}