#include <stdio.h>
#include <stdlib.h>

#include <curl/curl.h>

#include "./src/buffers.h"
#include "./src/http_utils.h"

int main(){
  struct curl_slist* headers = NULL;
  unsigned short respCode = 0;

  headers = curl_slist_append(headers, "Content-Type: text/plain");

  BinData* ptr = http_req(
    "https://echo.free.beeceptor.com",
    "POST",
    headers, NULL, 0, &respCode
  );

  printf("%lu bytes retrieved\n", (unsigned long) ptr ->size);
  printf("%s\n", ptr ->mem);
}
