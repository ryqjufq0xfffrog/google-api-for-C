build: sample.c src/buffers.o src/http_utils.o
	gcc -o sample sample.c src/buffers.o src/http_utils.o -lcurl
