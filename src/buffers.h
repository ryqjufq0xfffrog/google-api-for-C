/* Typedefs */

//  binaryData
typedef struct {
  char* mem;
  size_t size;
} BinData;

//  array
typedef struct {
  void** mem;
  size_t length;
  size_t size;
} Array;

/****************************/

/* Functions */

size_t appendBuf(void* data, size_t size4each, size_t nmemb, void* usrp);
