#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <cjson/cJSON.h>

#include "./buffers.h"
#include "./google.h"

void goog_global_init() {
  curl_global_init(CURL_GLOBAL_DEFAULT);

  return;
}

void goog_global_cleanup() {
  curl_global_cleanup();

  return;
}

void goog_free(void* ptr) {
  free(ptr);
  
  return;
}

GOOGLE_SLIST* 

void goog_free_auth(GOOGLE_AUTH* auth) {
}

char* createAuthUrl(GOOGLE_AUTH* auth, char** reqScopes) {
  char* URL;
  char params[2000];
  char* encodedScopes;
  char* encodedRedirect;
  char scopestr[1536];
  
  URL = (char* ) malloc(2048);
  if(!URL) return NULL;
  
  // Join scopes with space
  for(int i = 0; reqScopes[i]; i++) {
    sprintf(scopestr + strlen(scopestr), "%s ", reqScopes[i]);
  }
  
  // First URL-encode strings
  encodedScopes = curl_easy_escape(NULL, scopestr, strlen(scopestr));
  encodedRedirect = curl_easy_escape(NULL, auth ->redirect, strlen(auth ->redirect));
  
  // Generate Wuery String
  sprintf(params, "client_id=%s&response_type=code&scope=%s&state=%s&redirect_uri=%s",
      auth ->id, encodedScopes, auth ->state, encodedRedirect);
  // Finally generate URL
  sprintf(URL, "https://accounts.google.com/o/oauth2/v2/auth?%s", params);

  // Free memory
  curl_free(encodedScopes);
  curl_free(encodedRedirect);
  
  return URL;
}

int obtainTokenFromQuery(GOOGLE_AUTH* auth, char* queryStr) {
  // cURL variables
  CURL* curl;
  
  BinData data;
  CURLcode res;
  long resCode = 0;
  struct curl_slist* headers = NULL;
  unsigned short respCode = 0;
  char postBody[1280] = "";
  
  /*
   * Parse query string
   */
  // Buffers for parsing query string
  char authCode[257] = "";
  char scopeStr[1280] = "";
  char stateStr[512] = "";
  
  char* queryRm = queryStr;
  // Ignore "?"
  if(queryRm[0] == '?') queryRm++;
  for(short i = 0; queryRm[i]; i++){
    char* dest;
    // ?name1=value1&name2=value2.
    if(strncmp("code=", queryRm + i, 5) == 0) {
      dest = authCode;
      i += 5;
    }else if(strncmp("scope=", queryRm + i, 6) == 0) {
      dest = scopeStr;
      i += 6;
    }else if(strncmp("state=", queryRm + i, 6) == 0) {
      dest = stateStr;
      i += 6;
    }else if(strncmp("error=", queryRm + i, 6) == 0) {
      // Got error response
      fprintf(stderr, "OAuth2 Error");
      
      return -1;
    }
    
    // Then read its value
    for( ; queryRm[i] != '&'; i++) {
      if(queryRm[i] == '%') {
        // Decode url
        char ch = '\0';
        i++;
        ch = (queryRm[i] - 0x37 + (queryRm[i] >= '0' && queryRm[i] <= '9') * 7);
        i++;
        ch = ch * 16 + (queryRm[i] - 0x37 + (queryRm[i] >= '0' && queryRm[i] <= '9') * 7);
        
        dest[i] = ch;
      }
      else dest[i] = queryRm[i];
    }
    // Terminate the string
    dest[i] = '\0';
  }
  
  // Check if state string matches
  if(strcmp(stateStr, auth ->state)) {
    fprintf(stderr, "state mismatch; possible CORS attack\n");
#ifdef GOOGLE_STRICT
    return -1;
#endif
  }
  
  // Parse scope string
  short scopeN = 0;
  short j = 0;
  for(short i = 0; scopeStr[i]; i++) {
    if(scopeStr[i] == ' ') {
      // Null terminate
      auth ->scopes[scopeN][j] = '\0';
      
      // Read next
      scopeN++;
      auth ->scopes[scopeN] = (char* ) malloc(257);
    }else {
      auth ->scopes[scopeN][j] = scopeStr[i];
      j++;
    }
  }
  
  /*
   * Send oauth2 request to obtain token
   */
  curl = curl_easy_init();
  data.mem = malloc(1);
  data.size = 0;

  if(!curl) {
    fprintf(stderr, "curl init failed\n");

    return -1;
  }
  
  // SSL settings
  curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);
  
  // Store response data to "data"
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToBuf);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void* ) &data);
  
  // Set Headers
  headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  
  // POST Data
  sprintf(postBody, "code=%s&client_id=%s&client_secret=%s&redirect_uri=%s&grant_type=authorization_code",
      authCode, auth->id, auth->secret, auth->redirect);
  
  curl_easy_setopt(curl, CURLOPT_URL, "https://oauth2.googleapis.com/token");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postBody);
  
  // Execute request
  res = curl_easy_perform(curl);
  if(res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed; %s\n",
        curl_easy_strerror(res));
    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    
    return -1;
  }
  
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resCode);
  if(resCode != 200) {
    fprintf(stderr,
        "https://oauth2.googleapis.com/token returned %d", resCode);
    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    
    return -1;
  }
  
  // Cleanup cURL
  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);
  
  /* 
   * Parse obtained JSON
   */
  cJSON* json;
  json = cJSON_Parse(data.mem);
  
  // Free received data
  free(data.mem);
  
  // Check if correctly parsed
  if(json == NULL) {
    fprintf(stderr, "cJSON parse failed; %s",
        cJSON_GetErrorPtr());
    cJSON_Delete(json);
    return -1;
  }
  
  // Copy obtained token to struct GOOGLE_AUTH* auth
  cJSON* tokenJSON = cJSON_GetObjectItem(json, "access_token");
  cJSON* refreshJSON = cJSON_GetObjectItem(json, "refresh_token");
  
  if(cJSON_IsString(tokenJSON)) {
    strcpy(auth ->token, tokenJSON ->valuestring);
  }  
  if(cJSON_IsString(refreshJSON)) {
    strcpy(auth ->refresh, refreshJSON ->valuestring);
  }
  
  // Clean up
  cJSON_Delete(json);
  
  return 0;
}
