// setup RISC-V core instruction format type masks, and types 
typedef uint32_t opcode_mask_t;

const opcode_mask_t opcode_mask = 0x0000007f;
// define the various opcode possibilities
const opcode_mask_t opcode_I1 =   0x00000003;
const opcode_mask_t opcode_I2 =   0x0000000f;
const opcode_mask_t opcode_I3 =   0x00000013;
const opcode_mask_t opcode_I4 =   0x0000001b;
const opcode_mask_t opcode_I5 =   0x00000073;

const opcode_mask_t opcode_U1 =   0x00000017;
const opcode_mask_t opcode_U2 =   0x00000037;

const opcode_mask_t opcode_S1 =   0x00000023;

const opcode_mask_t opcode_R1 =   0x00000033;
const opcode_mask_t opcode_R2 =   0x0000003b;

const opcode_mask_t opcode_SB1 =  0x00000063;

const opcode_mask_t opcode_UJ1 =  0x0000006f;

typedef struct {
  uint32_t rd;
  uint32_t rd_shift;
  uint32_t funct3;
  uint32_t funct3_shift;
  uint32_t rs1;
  uint32_t rs1_shift;
  uint32_t rs2;
  uint32_t rs2_shift;
  uint32_t funct7;
  uint32_t funct7_shift;
} r_type_mask_t;

const r_type_mask_t r_type_mask = {
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

typedef struct {
  uint32_t rd;
  uint32_t rd_shift;
  uint32_t funct3;
  uint32_t funct3_shift;
  uint32_t rs1;
  uint32_t rs1_shift;
  uint32_t imm;
  uint32_t imm_shift;
} i_type_mask_t;

const i_type_mask_t i_type_mask = {
  .rd =      0x00000f80,
  .rd_shift = 7,
  .funct3 =  0x00007000,
  .funct3_shift = 12,
  .rs1 =     0x000f8000,
  .rs1_shift = 15,
  .imm =     0xfff00000,
  .imm_shift = 20,
};

typedef struct {
  uint32_t immlow;
  uint32_t immlow_shift;
  uint32_t funct3;
  uint32_t funct3_shift;
  uint32_t rs1;
  uint32_t rs1_shift;
  uint32_t rs2;
  uint32_t rs2_shift;
  uint32_t immhigh;
  uint32_t immhigh_shift;
} s_type_mask_t;

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

typedef struct {
  uint32_t immlow;
  uint32_t funct3;
  uint32_t funct3_shift;
  uint32_t rs1;
  uint32_t rs1_shift;
  uint32_t rs2;
  uint32_t rs2_shift;
  uint32_t immhigh;
} sb_type_mask_t;

const sb_type_mask_t sb_type_mask = {
  .immlow =  0x00000f80,
  .funct3 =    0x00007000,
  .funct3_shift = 12,
  .rs1 =       0x000f8000,
  .rs1_shift = 15,
  .rs2 =       0x01f00000,
  .rs2_shift = 20,
  .immhigh = 0xfe000000,
};

typedef struct {
  uint32_t rd;
  uint32_t rd_shift;
  uint32_t imm;
} u_type_mask_t;

const u_type_mask_t u_type_mask = {
  .rd =      0x00000f80,
  .rd_shift = 7,
  .imm =     0xfffff000,
};

typedef struct {
  uint32_t rd;
  uint32_t rd_shift;
  uint32_t imm;
} uj_type_mask_t;

const uj_type_mask_t uj_type_mask = {
  .rd =     0x00000f80,
  .rd_shift = 7,
  .imm =    0xfffff000,
};

enum instruction_format_t {
  R,
  I,
  S,
  SB,
  U,
  UJ,
  error
};

// hold all possible fields in a struct to make storage easier.
// its up to the execution funtions to know which fields to use.
typedef struct {
  enum instruction_format_t inst_format;
  uint32_t opcode;
  uint32_t rd;
  uint32_t funct3;
  uint32_t rs1;
  uint32_t rs2;
  uint32_t funct7;
  int imm;
} riscv_decoded_t;