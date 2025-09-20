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

size_t writeToBuf(char* data, size_t size4each, size_t nmemb, void* usrp);
size_t readFromBuf(char* data, size_t size4each, size_t nmemb, void* usrp);
