#include <stdint.h>


#include "6502.h"
#include "fetch.h"
#include "decoder.h"
#include "execute.h"





//User accessible registers
extern	uint8_t A;    //Accumulator
extern	uint8_t Y;    //Index Y
extern	uint8_t X;    //Index X
extern	uint8_t PCH;  //Program counter low
extern	uint8_t PCL;  //Program counter high
extern	uint16_t S;   //Stack pointer
extern	uint8_t P;    //Processor status register
	
//Internal registers
extern	uint8_t ABL;  //low bits of address
extern	uint8_t ABH;  //high bits of address
extern	uint8_t DB_DATA; //Data bus value
extern	uint8_t INS;    //opcode
extern	uint8_t OP1;    //operand 1
extern	uint8_t OP2;    //operand 2
extern	uint16_t PC;   //program counter

//Positions for PSR
extern	uint8_t C;     //carry bit
extern	uint8_t Z;     //zero bit
extern	uint8_t I;     //interrupt disable
extern	uint8_t D;     //decimal mode
extern	uint8_t B;     //break command
//0x20 is skipped by manufacturer
extern	uint8_t V;     //overflow bit
extern	uint8_t N;     //negative bit





extern void (*opcodes[256])();
extern int isset;

int issupported = 0;

int    //returns 0 on success and -1 on noo-existent opcode
execute(uint8_t op)
{
  if (isset == 0)
    set_opcodes();
  issupported = 0;
  opcodes[op]();
  if (issupported == -1)
    return -1;
  return 0;
}







//1 byte only instructions (implied addressing) and some exceptions for decoder


void  //NON-STANDARD!!! FOR DEBUGGING !!
wop()
{
  issupported = -1;
}

void //0xEA does nothing 
nop()
{
  //nothing happens
}

void //0x18  clear carry
clc()
{
  P = P & (0xff ^ C);
}


void //0x38    set carry
sec()
{
  P = P | C;
}

void //0xd8   clear decimal
cld()
{
  P = P & (0xff ^ D);
}


void //0xf8   set decimal
sed()
{
  P = P | D;
}

void //0x58    clear interrupt disable
cli()
{
  P = P & (0xff ^ I);
}


void //0x78     set interrupt disable
sei()
{
  P = P | I;
}

void //0xb8   clear overflow
clv()
{
  P = P & (0xff ^ V);
}


void   //0x00 set break
as_brk()
{
  P = P | I;
}


void //0xca   decrement X
dex()
{
  X--;
  eval_reg(X);
}

void //0x88  decrement Y
dey()
{
  Y--;
  eval_reg(Y);
}

void //0xe8  increment X
inx()
{
  X++;
  eval_reg(X);
}

void //0xc8  increment Y
iny()
{
  Y++;
  eval_reg(Y);
}

void //0xaa   transfer A to X
tax()
{
  X = A;
  eval_reg(X);
}


void //0x8a    transfer X to A
txa()
{
  A = X;
  eval_reg(A);
}

void  //0x98   transfer A to Y
tay()
{
  Y = A;
  eval_reg(Y);
}

void  //0xa8   transfer Y to A
tya()
{
  A = Y;
  eval_reg(A);
}

void //0x9a   transfer X to S
txs()
{
  S = X;
  eval_reg(S);
}

void //0xba  transfer S to X
tsx()
{
  X = S;
  eval_reg(X);
}


//TODO



// First group ops

void
and()
{
  A = A & DB_DATA;
  eval_reg(A);
}


void
ora()
{
  A = A | DB_DATA;
  eval_reg(A);
}


void
lda()
{
  A = DB_DATA;
  eval_reg(A);
}

void
eor()
{
  A = A ^ DB_DATA;
  eval_reg(A);
}



void
sta()
{
  DB_DATA = A;
  wr();
}


void
adc()
{
  if ((P & D) == 0) {    //If not decimal
    uint16_t temp = A + DB_DATA;
    if (P & C)
      temp++;
    if (temp > 0x255)
      set_flag(C, 1);
    else
      set_flag(C, 0);
    if ((A & 0x80) == (temp & 0x80))
      set_flag(V, 0);
    else
      set_flag(V, 1);
    A = temp & 0xff;
    eval_reg(A);
  }
  else {
    uint8_t t_a = A;
    int a_val = (A >> 4) * 10 + (A & 0xf);
    int d_val = (DB_DATA >> 4) * 10 + (DB_DATA & 0xf);
    int temp = a_val + d_val;
    if (P & C)
      temp++;
    if (temp >= 100) {
      temp -= 100;
      set_flag(C, 1);
    }
    else
      set_flag(C, 0);
    A = 0x10 * (temp / 10) + 0x1 * (temp % 10);
    if ((A & 0x80) == (t_a & 0x80))   //undocumented
      set_flag(V, 0);
    else
      set_flag(V, 1);
    eval_reg(A);     //undocumented
  }
}


void
sbc()
{
  if ((P & D) == 0) {    // If not decimal
    uint16_t temp = (~DB_DATA) + A;
    if (P & C)
      temp++;
    if (temp > 0xff)
      set_flag(C, 1);
    else
      set_flag(C, 0);
    if ((A & 0x80) == (temp & 0x80))
      set_flag(V, 0);
    else
      set_flag(V, 1);
    A = temp & 0xff;
    eval_reg(A);
  }
  else {
    uint8_t t_a = A;
    int a_val = (A >> 4) * 10 + (A & 0xf);
    int d_val = (DB_DATA >> 4) * 10 + (DB_DATA & 0xf);
    int temp = a_val - d_val - 1;
    if (P & C)
      temp++;
    if (temp < 0) {
      temp = - temp;
      set_flag(N, 1);
    }
    if (temp >= 100) {
      temp -= 100;
      set_flag(C, 0);
    }
    else
      set_flag(C, 1);
    A = 0x10 * (temp / 10) + 0x1 * (temp % 10);
    if ((A & 0x80) == (t_a & 0x80))   //undocumented                                                            
      set_flag(V, 0);
    else
      set_flag(V, 1);
    eval_reg(A);     //undocumented                                         
  }
}


void
cmp()
{
  uint8_t temp = DB_DATA - A;
  eval_reg(temp);
  set_flag(C, (DB_DATA <= A ? 1 : 0));
}




//Second class instructions


void
ldx()
{
  X = DB_DATA;
  eval_reg(X);
}


void
stx()
{
  DB_DATA = X;
  wr();
}

void
inc()
{
  DB_DATA++;
  eval_reg(DB_DATA);
  wr();
}

void
dec()
{
  DB_DATA--;
  eval_reg(DB_DATA);
  wr();
}


extern int acc_add;


void
asl()
{
  uint8_t *p = acc_add == 0 ? &DB_DATA : &A;
  if(((*p) & 0x80) == 0)
    set_flag(C, 1);
  else
    set_flag(C, 0);
  (*p) = (*p) << 1;
  eval_reg((*p));
  if (acc_add == 1)
    wr();
  acc_add = 0;
}


void
lsr()
{
  uint8_t *p = acc_add == 0	? &DB_DATA : &A;
  if(((*p) & 0x1) == 0)
    set_flag(C, 1);
  else
    set_flag(C, 0);
  (*p) = (*p) >> 1;
  eval_reg((*p));
  if (acc_add == 1)
    wr();
  acc_add = 0;
}


void
rol()
{
  uint8_t *p = acc_add == 0 ? &DB_DATA : &A;
  uint8_t t = P & C;
  if (((*p) & 0x80) != 0)
    set_flag(C, 1);
  else
    set_flag(C, 0);
  (*p) = (*p) << 1;
  if (t != 0)
    (*p)++;
  if(acc_add == 0)
    wr();
  acc_add = 0;
}


void
ror()
{
  uint8_t *p = acc_add == 0 ? &DB_DATA : &A;
  uint8_t t = P	& C;
  if (((*p) & 0x1) != 0)
    set_flag(C, 1);
  else
    set_flag(C, 0);
  (*p) = (*p) >> 1;
  if (t != 0)
    (*p)+= 0x80;
  if(acc_add == 0)
    wr();
  acc_add = 0;
}



//Done!

//third class instructions

void
ldy()
{
  Y = DB_DATA;
  eval_reg(Y);
}



void
sty()
{
  DB_DATA = Y;
  wr();
}


void
cpx()
{
  uint8_t temp = X - DB_DATA;
  eval_reg(temp);
  if (X >= DB_DATA)
    set_flag(C, 1);
  else
    set_flag(C, 0);
}



void
cpy()
{
  uint8_t temp = Y - DB_DATA;
  eval_reg(temp);
  if (Y >= DB_DATA)
    set_flag(C, 1);
  else
    set_flag(C, 0);
}


void
jmp_abs()
{
  PC = OP1 + 0x100 * OP2;
  //PCL and PCH are set in fetcher
}

void
jmp()
{
  ABL = OP1;
  ABH = OP2;
  rd();
  PCL = DB_DATA;
  if ((ABL & 0x80) != 0)
    ABH++;
  ABL++;
  rd();
  PCH = DB_DATA;
  PC = PCL + PCH * 0x100 - 1;
  //-1 compensating for fetcher;
  //PCL and PCH are already set, and will be reset with the same value in fetcher
}


void
bit()
{
  //first, we evaluating A
  //then we resetting N value with memory, instead of A
  uint8_t temp = DB_DATA & A;
  eval_reg(A);
  if (DB_DATA & 0x80)
    set_flag(N, 1);
  else
    set_flag(N, 0);
  if (DB_DATA & 0x40)
    set_flag(V, 1);
  else
    set_flag(V, 0);
}

//Done!

//Branch instructions

void
branch()
{
  PC ++;
  uint8_t flag;
  switch((INS >> 6) & 0x3) {
  case 0:
    flag = N;
    break;
  case 1:
    flag = V;
    break;
  case 2:
    flag = C;
    break;
  case 3:
    flag = Z;
    break;
  }
  uint16_t temp = 0;
  if ((test_flag(flag) && ((INS >> 5) & 0x1)) ||    //not (xor)  (1001)
      (!(test_flag(flag) || ((INS >> 5) & 0x1)))) {
    if (0x80 & OP1) {
      temp = (0xFF00 | OP1) + 1;
    }
    else
      temp = OP1;
    PC += temp - 1;
  }
      //-1  for negating increment in beggining of the function
}


void
jsr()
{
  PC += 2;
  PCH = PC >> 8;
  PCL = PC & 0xff;
  m_write(S--, PCH);
  m_write(S--, PCL);
  PC = OP1 + 0x100 * OP2 - 1;
}

void
rts()
{
  PCL = m_read(++S);
  PCH = m_read(++S);
  PC = PCL + PCH * 0x100;
}

void
pha()
{
  m_write(S--, A);
}

void
pla()
{
  A =m_read(S++);
  eval_reg(A);
}

void
php()
{
  m_write(S--, P);
}

void
plp()
{
  P = m_read(S++);
}
