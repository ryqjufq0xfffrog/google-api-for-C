#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./buffers.h"

size_t appendBuf(void* data, size_t size4each, size_t nmemb, void* usrp) {
  size_t size;
  BinData* bin;
  char* ptr;

  size = size4each * nmemb;
  bin = (BinData *)usrp;

  ptr = realloc(bin ->mem, bin ->size + size + 1);
  if(!ptr) {
    fprintf(stderr, "Out of memory\n");
    return 0;
  }
  
  bin ->mem = ptr;
  memcpy(bin ->mem + bin ->size, data, size);
  bin ->size += size;
  *(bin ->mem + bin ->size) = 0;

  return size;
}
