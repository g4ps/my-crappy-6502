#include <stdint.h>
#include "6502.h"
#include "memory.h"
#include "fetch.h"
#include "execute.h"
 
extern void (*opcodes[256])();

extern  uint8_t A;
extern  uint8_t X;
extern  uint8_t Y;
extern	uint8_t PCH;  //Program counter low
extern	uint8_t PCL;  //Program counter high
	
//Internal registers
extern	uint8_t ABL;  //low bits of address
extern	uint8_t ABH;  //high bits of address
extern	uint8_t DB_DATA; //Data bus value
extern	uint8_t INS;    //opcode
extern	uint8_t OP1;    //operand 1
extern	uint8_t OP2;    //operand 2
extern	uint16_t PC;   //program counter





//   Decoder
void  set_opcodes();





void
immediate()
{
  DB_DATA = OP1;
}


void
zeropage()
{
  ABH = 0;
  ABL = OP1;
  rd();
}

void
zeropage_x()
{
  ABH = 0;
  ABL = OP1 + X;
  rd();
}

void
zeropage_y()        //only for indexed zeropage for stx and ldx
{
  ABH = 0;
  ABL = OP1 + X;
  rd();
}

void
absolute()
{
  ABH = OP2;
  ABL = OP1;
  rd();
}

void
absolute_x()
{
  ABH =	OP2;
  ABL =	OP1;
  uint16_t temp = ABH * 0x100 + ABL + X;
  ABL =	temp & 0xff;
  ABH =	temp >>	8;
  rd();
}

void
absolute_y()
{
  ABH =	OP2;
  ABL =	OP1;
  uint16_t temp = ABH * 0x100 + ABL + Y;
  ABL =	temp & 0xff;
  ABH =	temp >>	8;
  rd();
}


void
indirect_x()
{
  ABH = 0;
  ABL = OP1 + X;
  rd();
  uint8_t temp = DB_DATA;
  ABL++;
  rd();
  ABL = temp;
  ABH = DB_DATA;
  rd();
}


void
indirect_y()
{
  uint8_t temp = OP1;
  ABH = m_read(temp + 1);
  ABL = m_read(temp);
  rd();
}


extern int acc_add;
void
accumulator()
{
  acc_add = 1;
  //we deal with accumulator addressing during execution
}

//TODO


void
first_class()
{
  uint8_t adr = (INS  >> 2) & 0x07;
  uint8_t in = (INS & 0xe0) >> 5;
  switch(adr) {
  case 0 :
    indirect_x();
    PC ++;
    break;
  case 1 :
    zeropage();
    PC ++;
    break;
  case 2:
    immediate();
    PC ++;
    break;
  case 3:
    absolute();
    PC += 2;
    break;
  case 4:
    indirect_y();
    PC ++;
    break;
  case 5:
    zeropage_x();
    PC ++;
    break;
  case 6:
    absolute_y();
    PC += 2;
    break;
  case 7:
    absolute_x();
    PC += 2;
    break;
  }
  switch(in) {
  case 0:
    ora();
    break;
  case 1:
    and();
    break;
  case 2:
    eor();
    break;
  case 3:
    adc();
    break;
  case 4:
    sta();
    break;
  case 5:
    lda();
    break;
  case 6:
    cmp();
    break;
  case 7:
    sbc();
    break;
  } 
}




static void
second_class()
{
  //Handle some special cases
  if (INS == 0x96) {   //stx zp indexed
    zeropage_y();
    PC++;
    stx();
    return;
  }
  if (INS == 0xb6) {   //ldx zp indexed
    zeropage_y();
    PC++;
    ldx();
    return;
  }
  if (INS == 0xbe) {   //ldx addressed
    absolute_y();
    PC += 2;
    ldx();
    return;
  }
  uint8_t ad, in;
  ad = (INS >> 2) & 0x7;
  in = (INS >> 5) & 0x7;
  switch(ad) {
  case 0:
    immediate();
    PC++;
    break;
  case 1:
    zeropage();
    PC++;
    break;
  case 2:
    accumulator();
    break;
  case 3:
    absolute();
    PC += 2;
    break;
  case 5:
    zeropage_x();
    PC++;
    break;
  case 7:
    absolute_x();
    PC += 2;
    break;
  }
  switch(in) {
  case 0:
    asl();
    break;
  case 1:
    rol();
    break;
  case 2:
    lsr();
    break;
  case 3:
    ror();
    break;
  case 4:
    stx();
    break;
  case 5:
    ldx();
    break;
  case 6:
    dec();
    break;
  case 7:
    inc();
    break;
  }
    
}


void
third_class()
{
  uint8_t ad, in;
  ad = (INS >> 2) & 0x7;
  in = (INS >> 5) & 0x7;
  switch(ad) {
  case 0:
    immediate();
    PC++;
    break;
  case 1:
    zeropage();
    PC++;
    break;
  case 3:
    absolute();
    PC += 2;
    break;
  case 5:
    zeropage_x();
    PC++;
    break;
  case 7:
    absolute();
    PC += 2;
    break;
  }
  switch(in) {
  case 1:
    bit();
    break;
  case 2:
    jmp_abs();
    break;
  case 3:
    jmp();
    break;
  case 4:
    sty();
    break;
  case 5:
    ldy();
    break;
  case 6:
    cpy();
    break;
  case 7:
    cpx();
    break;
  }
}






extern int isset;

void   //set_opcodes: sets array of pointers to functions with opcode functions (maybe can
              //make it better)
set_opcodes()
{
  isset = 1;   //only doing this function once
  for (int i = 0; i < 256; i++) {
    opcodes[i] = &wop;  //setting all to non-documented
  }

  //Implied addressing functions (1 byte operations)
  opcodes[0xea] = &nop;
  opcodes[0x18] = &clc;
  opcodes[0x38] = &sec;
  opcodes[0xd8] = &cld;
  opcodes[0xf8] = &sed;
  opcodes[0x58] = &cli;
  opcodes[0x78] = &sei;
  opcodes[0xb8] = &clv;
  opcodes[0x00] = &as_brk;
  opcodes[0xca] = &dex;
  opcodes[0x88] = &dey;
  opcodes[0xe8] = &inx;
  opcodes[0xc8] = &iny;
  opcodes[0xaa] = &tax;
  opcodes[0x8a] = &txa;
  opcodes[0x98] = &tay;
  opcodes[0xa8] = &tya;
  opcodes[0xba] = &tsx;
  opcodes[0x9a] = &txs;
  opcodes[0x48] = &pha;
  opcodes[0x08] = &php;
  opcodes[0x68] = &pla;
  opcodes[0x28] = &plp;
  opcodes[0x20] = &jsr;
  opcodes[0x60] = &rts;
  
  //there are some opcodes doing something to stack, TODO

  //first op class
  for (uint8_t i = 0; i < 8; i++) {
    for (uint8_t j = 0; j < 8; j++) {
      uint8_t temp = 0x20 * i + 0x4 * j + 1;
      opcodes[temp] = &first_class;
    }
  }
  opcodes[0x89] = &wop;
  /////////////////////////////////////////////////

  //second op class
  for (uint8_t i = 0; i < 8; i++) {
    for(uint8_t j = 0; j < 8; j++) {
      if (j == 4 || j == 6 || j == 0)
	continue;
      uint8_t temp = i * 0x20 + j * 0x4 + 2;
      if (temp != 0x8a && temp != 0xaa && temp != 0xca && temp != 0xea && temp != 0x9e)
	opcodes[temp] = &second_class;
    }
  }
  opcodes[0xa2] = &second_class;
  //////////////////////////////////////////////////


  //third op class
  for (uint8_t i = 5; i < 8; i++) {
    for (uint8_t j = 0; j < 4; j++) {
      if (j == 2)
	continue;
      uint8_t temp = i * 0x20 + j * 0x4;
      opcodes[temp] = &third_class;
    }
  }
  opcodes[0x84] = &third_class;
  opcodes[0x8c] = &third_class;
  opcodes[0x94] = &third_class;
  opcodes[0xb4] = &third_class;
  opcodes[0xbc] = &third_class;
  opcodes[0x6c] = &third_class;
  opcodes[0x4c] = &third_class;
  opcodes[0x2c] = &third_class;
  opcodes[0x24] = &third_class;
  //////////////////////////////////////////////////////

  //branch operations
  for (uint8_t i = 0; i < 4; i++) {
    for (uint8_t j = 0; j < 2; j++) {
      uint8_t temp = i * 0x40 + j * 0x20 + 0x10;
      opcodes[temp] = &branch;
    }
  }
}

