#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <cjson/cJSON.h>

#include "./src/google.h"
#include "./src/buffers.h"
#include "./src/http_utils.h"

CREDENTIALS readCredentials(char*);

int main(int argc, char** argv) {
//  struct curl_slist* headers = NULL;
//  unsigned short respCode = 0;
  char* credentialPath = NULL;

  CREDENTIALS credentials;
  
  for(int argi = 1; argi < argc; argi++) {
    if(strcmp(argv[argi], "-c") == 0) {
      argi++;
      if(argi < argc) credentialPath = *(argv + argi);
      else {
        fprintf(stderr, "missing filename after -c");
        return 1;
      }
    }
  }
  
  if(!credentialPath) {
    fprintf(stderr, "missing credential path");
    return 1;
  }

  credentials = readCredentials(credentialPath);
  char* scopes[4] = {"drive", "profile", "mail", NULL};
  char* authURL = createAuthUrl(credentials.id, scopes, "", "localhost");
  printf("%s\n%s\n", credentials.id, credentials.secret);
  printf("%s\n", authURL);
  
/*  headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlenc0ded");

  BinData* ptr = http_req2mem(
    "https://oauth2.googleapis.com/token",
    headers, &respCode
  );

  printf("%lu bytes retrieved\n", (unsigned long) ptr ->size);
  printf("%d\n", respCode);
  printf("%s\n", ptr ->mem);
  
  free(ptr ->mem);
*/

  return 0;
}

CREDENTIALS readCredentials(char* filePath) {
  FILE* fp;
  CREDENTIALS credentials;
  const char* _ID_STR = "client_id=";
  const char* _SECRET_STR = "client_secret=";

  if(fp = fopen(filePath, "r")) {
    char buffer[512];
    
    // Read line by line
    while(fgets(buffer, 512, fp)) {
      if(strncmp(buffer, _SECRET_STR, strlen(_SECRET_STR)) == 0) {
        // copy client secret
        strcpy(credentials.secret, buffer + strlen(_SECRET_STR));
        // remove \n at the end
        credentials.secret[strlen(credentials.secret) - 1] = '\0';
      }else if(strncmp(buffer, _ID_STR, strlen(_ID_STR)) == 0) {
        // copy client id
        strcpy(credentials.id, buffer + strlen(_ID_STR));
        // remove \n at the end
        credentials.id[strlen(credentials.id) - 1] = '\0';
      }
    }
    
    // Close file
    fclose(fp);
  }else {
    // Couldn't open credentials file
    fprintf(stderr, "Couldn't open %s", filePath);
  }

  return credentials;
}
