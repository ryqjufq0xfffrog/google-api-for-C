#include <stdio.h>
#include <stdlib.h>

#include <curl/curl.h>

#include "./buffers.h"
#include "./http_utils.h"

BinData* http_req2mem(char* url, struct curl_slist* headers, unsigned short* code) {
  BinData* ptr;

  ptr = (BinData*) malloc(sizeof(BinData));
  ptr ->mem = malloc(1);
  ptr ->size = 0;
  
  *code = http_req(url, "GET", headers, NULL, NULL, 0, writeToBuf, (void*) ptr);
  
  return ptr; 
}

unsigned short http_req(char* url, char* method, struct curl_slist* headers, size_t (*read_callback)(char*, size_t, size_t, void*), void* read_usrdata, size_t bodySize, size_t (*write_callback)(char*, size_t, size_t, void*), void* write_usrdata) {
  // load curl
  curl_global_init(CURL_GLOBAL_DEFAULT);

  CURL* curl;
  CURLcode res;
  unsigned short rtnCode = 0;

  // Create cURL handle
  curl = curl_easy_init();

  // Initialize failed?
  if(!curl) {
    curl_global_cleanup();
    fprintf(stderr, "curl init failed\n");

    return 0;
  }

  // Set url to fetch
  curl_easy_setopt(curl, CURLOPT_URL, url);

  // methods & headers
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  // send data if read_callback is given
  if(read_callback) {
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
    curl_easy_setopt(curl, CURLOPT_READDATA, read_usrdata);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t) bodySize);
  }

  // recieve data
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, write_usrdata);

  // SSL: Cache cert for a week in memory
  curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);

  // Send and if request fails stop the script
  res = curl_easy_perform(curl);
  if(res != CURLE_OK) {
    fprintf(stderr, "curl error: %s\n", curl_easy_strerror(res));
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return 0;
  }
  
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, (long*) &rtnCode);
  
  // Clean up
  curl_easy_cleanup(curl);
  curl_global_cleanup();
  curl_slist_free_all(headers);

  return rtnCode;
}
