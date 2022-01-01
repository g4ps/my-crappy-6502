#include <stdint.h>
#include "6502.h"
#include "memory.h"




int execute(uint8_t op);




//1 byte instructions


void nop();
void clc();
void sec();
void cld();
void sed();
void cli();
void sei();
void clv();
void as_brk();
void dex();
void dey();
void inx();
void iny();
void tax();
void txa();
void tay();
void tya();
void tsx();
void txs();
void wop();
void php();
void pha();
void plp();
void pla();
void jsr();
void rts();



//TODO




//First class instructions

void and();
void ora();
void lda();
void eor();
void sta();
void adc();
void sbc();
void cmp();

//Done!


//Second class instructions


void asl();
void rol();
void lsr();
void ror();
void stx();
void ldx();
void dec();
void inc();


//Third class instructions


void ldy();
void sty();
void cpx();
void cpy();
void jmp_abs();
void jmp();
void bit();


//branch instructions
void branch();
