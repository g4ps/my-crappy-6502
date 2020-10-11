#include <stdint.h>
#include <stdio.h>
#include "memory.h"


 //User accessible registers
uint8_t A = 0;    //Accumulator
uint8_t Y = 0;    //Index Y
uint8_t X = 0;    //Index X
uint8_t PCH = 0x2;  //Program counter low
uint8_t PCL = 0;  //Program counter high
uint16_t S = 0x1FF;   //Stack pointer
uint8_t P = 0;    //Processor status register



//Internal registers
uint8_t ABL = 0;  //low bits of address
uint8_t ABH = 0;  //high bits of address
uint8_t DB_DATA = 0; //Data bus value
uint8_t INS = 0;    //opcode
uint8_t OP1 = 0;    //operand 1
uint8_t OP2 = 0;    //operand 2
uint16_t PC = 0x200;   //program counter

int acc_add = 0;    //scecial variable for accumulator addressing;



//Positions for PSR
uint8_t C = 0x01;     //carry bit
uint8_t Z = 0x02;     //zero bit
uint8_t I = 0x04;     //interrupt disable
uint8_t D = 0x08;     //decimal mode
uint8_t B = 0x10;     //break command
//0x20 is skipped by manufacturer
uint8_t V = 0x40;     //overflow bit
uint8_t N = 0x80;     //negative bit


//  Supptoting functions


void
print_regs()
{
  printf("A:\t%X\nY:\t%X\nX:\t%X\nPCH:\t%X\nPCL:\t%X\nS:\t%X\nP:\t%X\n", A, Y, X, PCH, PCL, S, P);
  printf("PC:\t%X\n", PCH * 0x100 + PCL);
  printf("C: %X Z: %X I: %X D: %X B: %X V: %X N: %X\n", P & C, (P & Z) >> 1, (P & I) >> 2,
	 (P & D) >> 3, (P & B) >> 4, (P & V) >> 6, (P & N) >> 7);
  printf("ABL: %X\tABH: %X\tDB_DATA: %X\n", ABL, ABH, DB_DATA);
  printf("INS: %X\tOP1: %X\tOP2: %X\n\n", INS, OP1, OP2);
}

void
set_pc(uint16_t pc)
{
  PC = pc;
  PCL = pc & 0xff;
  PCH = pc >> 4;
}

void
eval_reg(uint8_t reg) //sets bits N and Z according to register value
{
  if (reg == 0)
    P = P | Z;
  else
    P = P & (0xff ^ Z);
  if (reg & 0x80)
    P = P | N;
  else
    P = P & (0xff ^ N);
}

void
set_flag(uint8_t flag, int val)
{
  if (val == 0)
    P = P & (0xff ^ flag);
  else
    P = P | flag;
}

int
test_flag(uint8_t flag)
{
  if (P & flag)
    return 1;
  else
    return 0;
}


void (*opcodes[256])();
int isset = 0;

int
isdone()
{
  if ( P & I )
    return 1;
  return 0;
}





