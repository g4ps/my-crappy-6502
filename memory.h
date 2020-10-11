#include <stdint.h>

#define MEM_SIZE 256 * 256

void set_mem(uint8_t*);
uint8_t m_read(uint16_t addr);
uint8_t m_write(uint16_t addr, uint8_t val);
