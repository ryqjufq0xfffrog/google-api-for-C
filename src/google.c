#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* createAuthUrl(char* id, char** scopes, char* state, char* redirect) {
  char* URL;
  char scopestr[1536];

  URL = (char* ) malloc(2048);
  if(!URL) return NULL;

  for(int i = 0; scopes[i]; i++) {
    sprintf(scopestr + strlen(scopestr), "%s\%%20", scopes[i]);
  }
  
  sprintf(URL, "https://accounts.google.com/o/oauth2/v2/auth/?client_id=%s&response_type=code&scope=%s&state=%s&redirect_uri=%s", id, scopestr, state, redirect);
  return URL;
}
