all= main.o 6502.o decoder.o execute.o fetch.o memory.o

proc6502: $(all)
	gcc $(all) -o proc6502
main.o: main.c
	gcc -c main.c
execute.o: execute.c execute.h
	gcc -c execute.c
6502.o: 6502.c 6502.h
	gcc -c 6502.c
decoder.o: decoder.c decoder.h
	gcc -c decoder.c
fetch.o: fetch.c fetch.h
	gcc -c fetch.c
memory.o: memory.c memory.h
	gcc -c memory.c


clean:
	rm $(all) proc6502
