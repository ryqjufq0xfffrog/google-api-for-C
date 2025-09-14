#include <stdio.h>
#include <stdlib.h>

#include <curl/curl.h>

#include "./src/buffers.h"
#include "./src/http_utils.h"

int main(){
  curl_slist* headers;
  unsigned short respCode = 0;

  headers = curl_slist_append

  BinData* ptr = http_req(
    "https://echo.free.beeceptor.com",
    "POST",
    NULL, NULL, 0, &respCode
  );

  printf("%lu bytes retrieved\n", (unsigned long) ptr ->size);
  printf("%s\n", ptr ->mem);
}
