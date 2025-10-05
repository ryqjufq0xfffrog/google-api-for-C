build: sample.c src/buffers.o src/http_utils.o src/google.o src/shell.o
	gcc sample.c src/buffers.o src/http_utils.o src/google.o src/shell.o -lcurl -lcjson -o sample.out

src/http_utils.o: src/buffers.o src/http_utils.c src/buffers.h
	gcc src/http_utils.c -c -o src/http_utils.o
src/buffers.o: src/buffers.c src/buffers.h
	gcc src/buffers.c -c -o src/buffers.o
src/shell.o: src/shell.c src/shell.h
	gcc src/shell.c -c -o src/shell.o
src/google.o: src/google.c src/google.h
	gcc src/google.c -c -o src/google.o
clean:
	rm src/*.o
	rm *.out
