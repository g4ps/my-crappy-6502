#include <stdint.h>
#include <stdio.h>
#include "memory.h"
#include "6502.h"


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
  printf("%s\n", ins_description(INS));
  printf("--------------------------------------------------------------\n\n");
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


char *ins_description(uint8_t ins)
{
  char *ret = "NOOP";
  switch(ins) {
  case 0x69:
  case 0x65:
  case 0x75:
  case 0x6D:
  case 0x7D:
  case 0x79:
  case 0x61:
  case 0x71:
    ret = "ADC Add Memory to Accumulator with Carry ";
    break;
  case 0x29:
  case 0x25:
  case 0x35:
  case 0x2D:
  case 0x3D:
  case 0x39:
  case 0x21:
  case 0x31:
    ret = "AND AND Memory with Accumulator ";
    break;
  case 0x0A:
  case 0x06:
  case 0x16:
  case 0x0E:
  case 0x1E:
    ret = "ASL Shift Left One Bit (Memory or Accumulator) ";
    break;
  case 0x90:
    ret = "BCC Branch on Carry Clear ";
    break;
  case 0xB0:
    ret = "BCS Branch on Carry Set ";
    break;
  case 0xF0:
    ret = "BEQ Branch on Result Zero ";
    break;
  case 0x24:
  case 0x2C:
    ret = "BIT Test Bits in Memory with Accumulator ";
    break;
  case 0x30:
    ret = "BMI Branch on Result Minus ";
    break;
  case 0xD0:
    ret = "BNE Branch on Result not Zero ";
    break;
  case 0x10:
    ret = "BPL Branch on Result Plus ";
    break;
  case 0x00:
    ret = "BRK Force Break ";
    break;
  case 0x50:
    ret = "BVC Branch on Overflow Clear ";
    break;
  case 0x70:
    ret = "BVS Branch on Overflow Set ";
    break;
  case 0x18:
    ret = "CLC Clear Carry Flag ";
    break;
  case 0xD8:
    ret = "CLD Clear Decimal Mode ";
    break;
  case 0x58:
    ret = "CLI Clear Interrupt Disable Bit ";
    break;
  case 0xB8:
    ret = "CLV Clear Overflow Flag ";
    break;
  case 0xC9:
  case 0xC5:
  case 0xD5:
  case 0xCD:
  case 0xDD:
  case 0xD9:
  case 0xC1:
  case 0xD1:
    ret = "CMP Compare Memory with Accumulator ";
    break;
  case 0xE0:
  case 0xE4:
  case 0xEC:
    ret = "CPX Compare Memory and Index X ";
    break;
  case 0xC0:
  case 0xC4:
  case 0xCC:
    ret = "CPY Compare Memory and Index Y ";
    break;
  case 0xC6:
  case 0xD6:
  case 0xCE:
  case 0xDE:
    ret = "DEC Decrement Memory by One ";
    break;
  case 0xCA:
    ret = "DEX Decrement Index X by One ";
    break;
  case 0x88:
    ret = "DEY Decrement Index Y by One ";
    break;
  case 0x49:
  case 0x45:
  case 0x55:
  case 0x4D:
  case 0x5D:
  case 0x59:
  case 0x41:
  case 0x51:
    ret = "EOR Exclusive-OR Memory with Accumulator ";
    break;
  case 0xE6:
  case 0xF6:
  case 0xEE:
  case 0xFE:
    ret = "INC Increment Memory by One ";
    break;
  case 0xE8:
    ret = "INX Increment Index X by One ";
    break;
  case 0xC8:
    ret = "INY Increment Index Y by One ";
    break;
  case 0x4C:
  case 0x6C:
    ret = "JMP Jump to New Location ";
    break;
  case 0x20:
    ret = "JSR Jump to New Location Saving Return Address ";
    break;
  case 0xA9:
  case 0xA5:
  case 0xB5:
  case 0xAD:
  case 0xBD:
  case 0xB9:
  case 0xA1:
  case 0xB1:
    ret = "LDA Load Accumulator with Memory ";
    break;
  case 0xA2:
  case 0xA6:
  case 0xB6:
  case 0xAE:
  case 0xBE:
    ret = "LDX Load Index X with Memory ";
    break;
  case 0xA0:
  case 0xA4:
  case 0xB4:
  case 0xAC:
  case 0xBC:
    ret = "LDY Load Index Y with Memory ";
    break;
  case 0x4A:
  case 0x46:
  case 0x56:
  case 0x4E:
  case 0x5E:
    ret = "LSR Shift One Bit Right (Memory or Accumulator) ";
    break;
  case 0xEA:
    ret = "NOP No Operation ";
    break;
  case 0x09:
  case 0x05:
  case 0x15:
  case 0x0D:
  case 0x1D:
  case 0x19:
  case 0x01:
  case 0x11:
    ret = "ORA OR Memory with Accumulator ";
    break;
  case 0x48:
    ret = "PHA Push Accumulator on Stack ";
    break;
  case 0x08:
    ret = "PHP Push Processor Status on Stack ";
    break;
  case 0x68:
    ret = "PLA Pull Accumulator from Stack ";
    break;
  case 0x28:
    ret = "PLP Pull Processor Status from Stack ";
    break;
  case 0x2A:
  case 0x26:
  case 0x36:
  case 0x2E:
  case 0x3E:
    ret = "ROL Rotate One Bit Left (Memory or Accumulator) ";
    break;
  case 0x6A:
  case 0x66:
  case 0x76:
  case 0x6E:
  case 0x7E:
    ret = "ROR Rotate One Bit Right (Memory or Accumulator) ";
    break;
  case 0x40:
    ret = "RTI Return from Interrupt ";
    break;
  case 0x60:
    ret = "RTS Return from Subroutine ";
    break;
  case 0xE9:
  case 0xE5:
  case 0xF5:
  case 0xED:
  case 0xFD:
  case 0xF9:
  case 0xE1:
  case 0xF1:
    ret = "SBC Subtract Memory from Accumulator with Borrow ";
    break;
  case 0x38:
    ret = "SEC Set Carry Flag ";
    break;
  case 0xF8:
    ret = "SED Set Decimal Flag ";
    break;
  case 0x78:
    ret = "SEI Set Interrupt Disable Status ";
    break;
  case 0x85:
  case 0x95:
  case 0x8D:
  case 0x9D:
  case 0x99:
  case 0x81:
  case 0x91:
    ret = "STA Store Accumulator in Memory ";
    break;
  case 0x86:
  case 0x96:
  case 0x8E:
    ret = "STX Store Index X in Memory ";
    break;
  case 0x84:
  case 0x94:
  case 0x8C:
    ret = "STY Sore Index Y in Memory ";
    break;
  case 0xAA:
    ret = "TAX Transfer Accumulator to Index X ";
    break;
  case 0xA8:
    ret = "TAY Transfer Accumulator to Index Y ";
    break;
  case 0xBA:
    ret = "TSX Transfer Stack Pointer to Index X ";
    break;
  case 0x8A:
    ret = "TXA Transfer Index X to Accumulator ";
    break;
  case 0x9A:
    ret = "TXS Transfer Index X to Stack Register ";
    break;
  default:
    ret = "No such operation\n";
  }
  return ret;
}
