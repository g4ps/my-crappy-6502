#include <stdint.h>



void set_pc(uint16_t);
void print_regs();
void eval_reg(uint8_t);
void set_flag(uint8_t, int);
int test_flag(uint8_t);
int isdone();
