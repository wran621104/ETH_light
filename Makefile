CC=g++
CFLAGS=-I.
DEPS = sha3.h compiler.h data_sizes.h my_endian.h fnv.h ethash.h internal.h
OBJ = sha3.o internal.o main.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

eth_test: $(OBJ)
	g++ -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o
