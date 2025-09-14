BinData* http_req(char* url, char* method, struct curl_slist* headers, size_t (*read_callback)(char*, size_t, size_t, void*), size_t bodySize, unsigned short* code);
