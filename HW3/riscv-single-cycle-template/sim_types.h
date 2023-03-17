// setup RISC-V core instruction format type masks, and types 
typedef struct {
  int opcode;
  int opcode_shift;
  int rd;
  int rd_shift;
  int funct3;
  int func3_shift;
  int rs1;
  int rs1_shift;
  int rs2;
  int rs2_shift;
  int funct7;
  int funct7_shift;
} r_type_mask_t;

const r_type_mask_t r_type_mask = {
  .opcode =  0b00000000000000000000000001111111,
  .opcode_shift = 0,
  .rd =      0b00000000000000000000111110000000,
  .rd_shift = 7,
  .funct3 =  0b00000000000000000111000000000000,
  .func3_shift = 12,
  .rs1 =     0b00000000000011111000000000000000,
  .rs1_shift = 15,
  .rs2 =     0b00000001111100000000000000000000,
  .rs2_shift = 20,
  .funct7 =  0b11111110000000000000000000000000,
  .funct7_shift = 25,
};

typedef struct {
  int opcode;
  int rd;
  int funct3;
  int rs1;
  int rs2;
  int funct7;
} r_type_t;

typedef struct {
  int opcode;
  int opcode_shift;
  int rd;
  int rd_shift;
  int funct3;
  int funct3_shift;
  int rs1;
  int rs1_shift;
  int imm;
} i_type_mask_t;

const i_type_mask_t i_type_mask = {
  .opcode =  0b00000000000000000000000001111111,
  .opcode_shift = 0,
  .rd =      0b00000000000000000000111110000000,
  .rd_shift = 7,
  .funct3 =  0b00000000000000000111000000000000,
  .funct3_shift = 12,
  .rs1 =     0b00000000000011111000000000000000,
  .rs1_shift = 15,
  .imm =     0b11111111111100000000000000000000,
};

typedef struct {
  int opcode;
  int rd;
  int funct3;
  int rs1;
  int imm;
} i_type_t;

typedef struct {
  int opcode;
  int opcode_shift;
  int imm40;
  int funct3;
  int funct3_shift;
  int rs1;
  int rs1_shift;
  int rs2;
  int rs2_shift;
  int imm115;
} s_type_mask_t;

const s_type_mask_t s_type_mask = {
  .opcode =  0b00000000000000000000000001111111,
  .opcode_shift = 0,
  .imm40 =   0b00000000000000000000111110000000,
  .funct3 =  0b00000000000000000111000000000000,
  .funct3_shift = 12,
  .rs1 =     0b00000000000011111000000000000000,
  .rs1_shift = 15,
  .rs2 =     0b00000001111100000000000000000000,
  .rs2_shift = 20,
  .imm115 =  0b11111110000000000000000000000000,
};

typedef struct {
  int opcode;
  int funct3;
  int rs1;
  int rs2;
  int imm;
} s_type_t;

typedef struct {
  int opcode;
  int opcode_shift;
  int imm41_11;
  int funct3;
  int funct3_shift;
  int rs1;
  int rs1_shift;
  int rs2;
  int rs2_shift;
  int imm12_105;
} sb_type_mask_t;

const sb_type_mask_t sb_type_mask = {
  .opcode =   0b00000000000000000000000001111111,
  .opcode_shift = 0,
  .imm41_11 =  0b00000000000000000000111110000000,
  .funct3 =    0b00000000000000000111000000000000,
  .funct3_shift = 12,
  .rs1 =       0b00000000000011111000000000000000,
  .rs1_shift = 15,
  .rs2 =       0b00000001111100000000000000000000,
  .rs2_shift = 20,
  .imm12_105 = 0b11111110000000000000000000000000,
};

typedef struct {
  int opcode;
  int funct3;
  int rs1;
  int rs2;
  int imm;
} sb_type_t;

typedef struct {
  int opcode;
  int opcode_shift;
  int rd;
  int rd_shift;
  int imm;
} u_type_mask_t;

const u_type_mask_t u_type_mask = {
  .opcode =  0b00000000000000000000000001111111,
  .opcode_shift = 0,
  .rd =      0b00000000000000000000111110000000,
  .rd_shift = 7,
  .imm =     0b11111111111111111111000000000000,
};

typedef struct {
  int opcode;
  int rd;
  int imm;
} u_type_t;

typedef struct {
  int opcode;
  int opcode_shift;
  int rd;
  int rd_shift;
  int imm;
} uj_type_mask_t;

const uj_type_mask_t uj_type_mask = {
  .opcode = 0b00000000000000000000000001111111,
  .opcode_shift = 0,
  .rd =     0b00000000000000000000111110000000,
  .rd_shift = 7,
  .imm =    0b11111111111111111111000000000000,
};

typedef struct {
  int opcode;
  int rd;
  int imm;
} uj_type_t;