SRC_FILES  = main.c 6502.c decoder.c execute.c fetch.c memory.c
SRC_DIR = src
SRC = $(SRC_FILES:%=$(SRC_DIR)/%)
OBJS = $(SRC:%.c=%.o)

INC_FLAG= -Iinclude/


CFLAGS = $(INC_FLAG)

CFLAGS += -g #debugging


proc6502: $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o proc6502

clean:
	rm $(OBJS) proc6502
