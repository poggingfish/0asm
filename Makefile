CC=g++
v=v
CFLAGS= -c -Wall -O2
VFLAGS= -prod
build: mkdir build/oasm.o build/execute.o build/parser.o bin/1asm.out
	$(CC) build/oasm.o build/execute.o build/parser.o -o bin/0asm.out
build/oasm.o: src/oasm.cc
	$(CC) $(CFLAGS) src/oasm.cc -o build/oasm.o
build/execute.o: src/execute.cc
	$(CC) $(CFLAGS) src/execute.cc -o build/execute.o
build/parser.o: src/parser.cc
	$(CC) $(CFLAGS) src/parser.cc -o build/parser.o
clean:
	rm -r build
	rm -r bin
mkdir:
	mkdir -p build
	mkdir -p bin
bin/1asm.out: 1asm/src/*
	$(v) $(VFLAGS) 1asm/src -o bin/1asm.out