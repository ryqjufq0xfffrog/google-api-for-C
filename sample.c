#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./src/google.h"
#include "./src/buffers.h"
#include "./src/shell.h"

GOOGLE_AUTH readCredentials(char*);

int main(int argc, char** argv) {
  char* credentialPath = NULL;
  char* authCode;
  
  GOOGLE_AUTH auth;
  
  goog_global_init();
  
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
    fprintf(stderr, "missing credential path\n");
    return 1;
  }
  
  auth = readCredentials(credentialPath);
  char* scopes[2] = {"https://www.googleapis.com/auth/drive.appdata", NULL};
  auth.scopes = scopes;
  char* authURL = createAuthUrl(&auth, "");
  printf("%s\n", authURL);
  
  authCode = getpasswd("Paste the Authorization Code: ", 257);
  obtainTokenFromCode(&auth, authCode);
  
  // Show obtained access token and refresh token
  printf("Access token: %s\nRefresh_token: %s\n",
      auth.token, auth.refresh);
  
  goog_global_cleanup();
  
  return 0;
}
 
GOOGLE_AUTH readCredentials(char* filePath) {
  FILE* fp;
  GOOGLE_AUTH credentials;
  const char* _ID_STR = "client_id=";
  const char* _SECRET_STR = "client_secret=";
  const char* _R_URL_STR = "redirect_uri=";
  
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
      }else if(strncmp(buffer, _R_URL_STR, strlen(_R_URL_STR)) == 0) {
        // copy client id
        strcpy(credentials.redirect, buffer + strlen(_R_URL_STR));
        // remove \n at the end
        credentials.redirect[strlen(credentials.redirect) - 1] = '\0';
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
