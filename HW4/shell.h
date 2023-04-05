/***************************************************************/
/*                                                             */
/*   RISCV Instruction Level Simulator                          */
/*                                                             */
/***************************************************************/

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*          DO NOT MODIFY THIS FILE!                            */
/*          You should only change sim.c!                       */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#ifndef _SIM_SHELL_H_
#define _SIM_SHELL_H_

#include <inttypes.h>
#define FALSE 0
#define TRUE  1

#define RISCV_REGS 32

uint32_t mem_read_32(uint32_t address);
void     mem_write_32(uint32_t address, uint32_t value);

extern uint32_t stat_cycles, stat_inst_retire, stat_inst_fetch, stat_flush;

#endif
