#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>



#include "6502.h"
#include "memory.h"
#include "fetch.h"
#include "decoder.h"
#include "execute.h"



void print_supported();
//void exec_prog(uint8_t*, int);
void check_memory(uint8_t*, int);

//extern LF;


int
main(int argc, char *argv[])
{
  int c;
  int v = 0;
  char *ch_offset = NULL;
  char *filename = NULL;
  char *pausetime = NULL;
  while ((c = getopt(argc, argv, "f:vs:o:")) != EOF) {
    switch(c) {
    case 'f':
      filename = optarg;
      break;
    case 'v':
      v = 1;
      break;
    case 's':
      pausetime = optarg;
      break;
    case 'o':
      ch_offset = optarg;
    default:
      fprintf(stderr, "Usage: %s [-f program][-v][-s pausetime(default 0)\n", argv[0]);
      exit(1);
    }
  }

  
  uint8_t* m = malloc(MEM_SIZE);
  if (m == NULL) {
    fprintf(stderr, "Something is wrong with malloc\n");
    exit(1);
  }
  set_mem(m);
  memset(m, 0, MEM_SIZE);

  int offset = 0x200;
  if (ch_offset != NULL) {
    int	o_len;
    for	(o_len = 0; isxdigit(ch_offset[o_len]); o_len++)
      ;
    if (o_len != strlen(ch_offset)) {
      fprintf(stderr, "optinon after -o is not a number\n");
      exit(4);
    }
    offset = (int)strtol(ch_offset, (char **)NULL, 10);;
  }

  set_pc(offset);

  
  if (filename != NULL) {
    int fd;
    if ((fd = open(filename, O_RDONLY)) < 0) {
      perror(filename);
      exit(2);
    }
    m += offset;
    if (read(fd, m, MEM_SIZE) < 0) {
      perror("read");
      exit(3);
    }
  }

  int pt = 0;

  if (pausetime != NULL) {
    int plen;
    for (plen = 0; isdigit(pausetime[plen]) ; plen++)
      ;
    if (plen != strlen(pausetime)) {
      fprintf(stderr, "non-number after -s\n");
      exit(4);
    }
    pt = atoi(pausetime);
  }
      

  int ins_done = 0;
  while(!isdone()) {             //Main action
    execute(fetch());
    ins_done++;
    if (v == 1) {
      print_regs();
    }
    if (pausetime)
      sleep(pt);
    
  }
      
  printf("Instructons completed: %d\n", ins_done);
  return 0;
}


//Support functions (for debugging)


void
print_supported()
{
  for (int i = 0; i < 256; i++) {
    printf("%X: ", i);
    if (execute(i) == 0)
      printf("supported\n");
    else
      printf("-----\n");
  }
  printf("\n\n\n");
}


void
check_memory(uint8_t* arr, int size)
{
  for(int i = 0; i < size; i++) {
    if (arr[i] != 0) {
      printf("%X: %X ", i, arr[i]);
    }
  }
  printf("\n\n\n");
}
