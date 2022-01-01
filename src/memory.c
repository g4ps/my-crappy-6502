#include <stdint.h>
#include "memory.h"


uint8_t *mem;

void
set_mem(uint8_t* add)
{
  mem = add;
}


uint8_t
m_read(uint16_t add)
{
  return mem[add];
}

uint8_t
m_write(uint16_t add, uint8_t val)
{
  uint8_t temp = mem[add];
  mem[add] = val;
  return temp;
}


