#define DEBUG_BOOL 1
#define DEBUG if(DEBUG_BOOL)

#include <inttypes.h>

enum instruction_format_t decode_opcode(uint32_t instruction);

riscv_decoded_t decode_r_type(uint32_t instruction);
riscv_decoded_t decode_i_type(uint32_t instruction);
riscv_decoded_t decode_s_type(uint32_t instruction);
riscv_decoded_t decode_sb_type(uint32_t instruction);
riscv_decoded_t decode_u_type(uint32_t instruction);
riscv_decoded_t decode_uj_type(uint32_t instruction);