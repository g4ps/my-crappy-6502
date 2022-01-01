#include <stdint.h>

#include "6502.h"
#include "memory.h"
#include "fetch.h"



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



//Fetching



uint8_t
fetch()
{
  //PC = PCL + PCH * 0x100;
  INS = m_read(PC);
  OP1 = m_read(PC + 1);
  OP2 = m_read(PC + 2);
  PC++;
  PCL = 0xff & PC;
  PCH = PC >> 8;
  return INS;
}



void
rd()
{
  DB_DATA = m_read(ABL + 0x100 * ABH);
}


void
wr()
{
  m_write(ABL + ABH * 0x100, DB_DATA);
}

