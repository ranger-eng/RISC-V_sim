#include <inttypes.h>

// setup RISC-V core instruction format type masks, and types 
typedef uint32_t opcode_mask_t;

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

typedef struct {
  uint32_t rd;
  uint32_t rd_shift;
  uint32_t imm;
} u_type_mask_t;

typedef struct {
  uint32_t rd;
  uint32_t rd_shift;
  uint32_t imm;
} uj_type_mask_t;

enum instruction_format_t {
  R,
  I,
  S,
  SB,
  U,
  UJ,
  noop
};

// hold all possible fields in a struct to make storage easier.
// its up to the execution funtions to know which fields to use.
typedef struct {
  enum instruction_format_t inst_format;
  uint32_t opcode;
  uint32_t rd;
  uint32_t funct3;
  uint32_t rs1;
  uint32_t rs1_value;
  uint32_t rs2;
  uint32_t rs2_value;
  uint32_t funct7;
  int imm;
} riscv_decoded_t;