CC = gcc
DELCMD = rm
INCDIR = -I/usr/local/include -I/usr/include
LFLAGS = -lcurl -ljson-c
BUILDSRC = sample.c src/buffers.o src/http_utils.o src/google.o src/shell.o

build: $(BUILDSRC)
	$(CC) $(BUILDSRC) $(INCDIR) $(LFLAGS) -o sample.out
src/http_utils.o: src/buffers.o src/http_utils.c src/buffers.h
	$(CC) src/http_utils.c -c -o src/http_utils.o
src/buffers.o: src/buffers.c src/buffers.h
	$(CC) src/buffers.c -c -o src/buffers.o
src/shell.o: src/shell.c src/shell.h
	$(CC) src/shell.c -c -o src/shell.o
src/google.o: src/google.c src/google.h
	$(CC) src/google.c -c -o src/google.o
clean:
	$(DELCMD) src/*.o
	$(DELCMD) *.out
