#include <stdio.h>
#include <stdlib.h>

#include <curl/curl.h>

#include "./buffers.h"
#include "./http_utils.h"

BinData* http_req(char* url, char* method, struct curl_slist* headers, size_t (*read_callback)(char*, size_t, size_t, void*), size_t bodySize, unsigned short* code){
  // load curl
  curl_global_init(CURL_GLOBAL_DEFAULT);

  CURL* curl;
  CURLcode res;
  BinData* ptr;

  ptr = (BinData*) malloc(sizeof(BinData));
  ptr ->mem = malloc(1);
  ptr ->size = 0;

  // Create cURL handle
  curl = curl_easy_init();

  // Initialize failed?
  if(!curl) {
    curl_global_cleanup();
    fprintf(stderr, "curl init failed\n");

    return NULL;
  }

  // Set url to fetch
  curl_easy_setopt(curl, CURLOPT_URL, url);

  // methods & headers
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  // send data if read_callback is given
  if(read_callback) {
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t) bodySize);
  }

  // recieve data
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, appendBuf);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) ptr);

  // SSL: Cache cert for a week in memory
  curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);

  // Send and if request fails stop the script
  res = curl_easy_perform(curl);
  if(res != CURLE_OK) {
    fprintf(stderr, "curl error: %s\n", curl_easy_strerror(res));
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return NULL;
  }
  
  if(code) {
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, (long*) code);
  }

  // Clean up
  curl_easy_cleanup(curl);
  curl_global_cleanup();
  curl_slist_free_all(headers);

  return ptr;
}
