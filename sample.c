#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./src/google.h"
#include "./src/buffers.h"
#include "./src/shell.h"

GOOGLE_AUTH readCredentials(char*);

int main(int argc, char** argv) {
  char* credentialPath = NULL;
  char* queryStr;
  
  GOOGLE_AUTH auth;
  
  goog_global_init();
  
  for(int argi = 1; argi < argc; argi++) {
    if(strcmp(argv[argi], "-c") == 0) {
      argi++;
      if(argi < argc) credentialPath = *(argv + argi);
      else {
        fprintf(stderr, "missing filename after -c\n");
        return 1;
      }
    }
  }
  
  if(!credentialPath) {
    fprintf(stderr, "missing credential path\n");
    return 1;
  }
  
  auth = readCredentials(credentialPath);
  char* scopes[2] = {"https://www.googleapis.com/auth/drive.appdata", NULL};
  auth.state = "";
  char* authURL = createAuthUrl(&auth, scopes);
  printf("%s\n", authURL);
  
  queryStr = getpasswd("Paste the query string: ", 1280);
  obtainTokenFromCode(&auth, queryStr);
  
  // Show obtained access token and refresh token
  printf("Access token: %s\nRefresh_token: %s\n",
      auth.token, auth.refresh);
  
  goog_global_cleanup();
  
  return 0;
}
 
GOOGLE_AUTH readCredentials(char* filePath) {
  FILE* fp;
  GOOGLE_AUTH auth;
  
  if(fp = fopen(filePath, "r")) {
    char buffer[512];
    
    // Read line by line
    while(fgets(buffer, 512, fp)) {
      char* dest;
      char* readPtr;
      if(strncmp(buffer, "client_secret=", 14) == 0) {
        // client secret
        dest = auth.secret;
        readPtr = buffer + 14;
      }else if(strncmp(buffer, "client_id", 9) == 0) {
        // client id
        dest = auth.id;
        readPtr = buffer + 9;
      }else if(strncmp(buffer, "redirect_uri=", 13) == 0) {
        // redirect url
        dest = auth.redirect;
        readPtr = buffer + 13;
      }
      strcpy(dest, readPtr);
      // remove \n at the end
      dest[strlen(dest) - 1] = '\0';
    }
    
    // Close file
    fclose(fp);
  }else {
    // Couldn't open credentials file
    fprintf(stderr, "Couldn't open %s", filePath);
  }
  
  return credentials;
}
