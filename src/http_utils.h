BinData* http_req2mem(char* url, struct curl_slist* headers, unsigned short* code);

unsigned short http_req(char* url, char* method, struct curl_slist* headers, size_t (*read_callback)(char*, size_t, size_t, void*), void* read_usrdata, size_t bodySize, size_t (*write_callback)(char*, size_t, size_t, void*), void* write_usrdata);

