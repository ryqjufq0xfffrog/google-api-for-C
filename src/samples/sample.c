#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../google.h"
#include "../buffers.h"
#include "../shell.h"

int main(int argc, char** argv) {
  char* credentialPath = NULL;
  char* state = "hello123456";
  
  GOOGLE_CRED* cred;
  GOOGLE_AUTH* auth;
  GOOGLE_SLIST* reqScopes = NULL;
  
  goog_global_init();
  
  for(int argi = 1; argi < argc; argi++) {
    if(strcmp(argv[argi], "-c") == 0) {
      argi++;
      if(argi < argc) credentialPath = *(argv + argi);
      else {
        fprintf(stderr, "missing filename after -c\n");
        return 1;
      }
    }else if(strcmp(argv[argi], "--state") == 0) {
      argi++;
      if(argi < argc) state = *(argv + argi);
      else {
        fprintf(stderr, "missing state string after --state\n");
        return 1;
      }
    }
  }
  
  if(!credentialPath) {
    fprintf(stderr, "missing credential path\n");
    return 1;
  }
  
  cred = readCredentialsFromFile(credentialPath);
  auth = new_GoogleAuth(cred);
  
  reqScopes = goog_slist_append(reqScopes, "https://www.googleapis.com/auth/drive.appdata");
  char* authURL = createAuthUrl(auth, reqScopes, NULL, state, 1, "");
  goog_list_free_all(reqScopes);
  printf("%s\n", authURL);
  
  char* queryStr;
  queryStr = getpasswd("Paste the query string: ", 1280);
  obtainTokenFromQuery(auth, queryStr, NULL, state);
  
  // Show obtained access token and refresh token
  printf("Access token: %s (expire: %ld)\nRefresh_token: %s (expire: %ld)\n",
      auth ->token, auth ->token_expire, auth ->refresh, auth ->refresh_expire);
  
  goog_free_auth(auth);
  goog_global_cleanup();
  
  return 0;
}
