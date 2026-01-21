#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <curl/curl.h>
#include <json-c/json.h>

#include "./buffers.h"
#include "./google.h"

/*
 * creates an redirect URL for Google's authorization server
 *
 * Params:
 * auth : contains credentials
 * reqScopes : the list of scopes that contains all you need
 * state : string; any value to maintain state
 * offline : boolean; set 1 to get refresh token
 * optional : string; optional query parameters.
 * * Must start with & like this: "&login_hint=sample.gmail.com&prompt=none"
 */
char* createAuthUrl(GOOGLE_AUTH* auth, GOOGLE_SLIST* reqScopes, char* state, int offline, char* optional) {
  char* URL;
  char params[2000];
  char* scopes_encoded;
  char* redirect_encoded;
  char scopestr[1536] = "";

  URL = (char* ) malloc(2048);
  if(!URL) return NULL;

  // Join scopes with space
  for( ; reqScopes; reqScopes = reqScopes ->next) {
    sprintf(scopestr + strlen(scopestr), "%s ", reqScopes ->str);
  }

  // first URL-encode strings
  scopes_encoded = curl_easy_escape(NULL, scopestr, strlen(scopestr));
  redirect_encoded = curl_easy_escape(NULL, auth -> cred ->redirect, strlen(auth -> cred ->redirect));
  // generate Query String
  if(state) {
    // state is not NULL
    sprintf(params, "client_id=%s&response_type=code&scope=%s&access_type=%s&redirect_uri=%s&state=%s%s",
        auth ->cred ->id, scopes_encoded, offline? "offline": "online", redirect_encoded, state, optional);
  }else {
    // state not given
    sprintf(params, "client_id=%s&response_type=code&scope=%s&access_type=%s&redirect_uri=%s%s",
        auth ->cred ->id, scopes_encoded, offline? "offline": "online", redirect_encoded, optional);
  }
  // free encoded strings
  curl_free(scopes_encoded);
  curl_free(redirect_encoded);

  // generate the result
  sprintf(URL, "https://accounts.google.com/o/oauth2/v2/auth?%s", params);

  return URL;
}

/*
 * parse scopes (split with " ")
 *
 * Params :
 * auth : auth ->scopes will be the parsed scopes
 * scopeStr : scopes joined by " "
 */
int parseScopes(GOOGLE_AUTH* auth, const char* scopeStr) {
  // Free previous
  if(auth ->scopes) {
    goog_list_free_all(auth ->scopes);
    auth ->scopes = NULL;
  }

  short j = 0;
  char scopeBuf[256];
  for(short i = 0; ; i++) {
    if(scopeStr[i] == ' ' || scopeStr[i] == '\0') {
      // End of a scope?
      // Null terminate buf
      scopeBuf[j] = '\0';
      auth ->scopes = goog_slist_append(auth ->scopes, scopeBuf);

      // Read next
      j = 0;
      // EOS?
      if(scopeStr[i] == '\0') break;
    }else {
      scopeBuf[j] = scopeStr[i];
      j++;
      if(j > 255) {
        fprintf(stderr, "One of the received scope was invalid: longer than 256 bytes\n");

        return -1;
      }
    }
  }
  return 0;
}

/*
 * Send a request to https://oauth2.googleapis.com/token
 * to retrieve tokens.
 *
 * Params :
 * auth : auth ->token (and possibly auth ->refresh) will be assigned
 * postBody : the request body
 */
int exchangeToken(GOOGLE_AUTH* auth, char* postBody) {
  // cURL variables
  CURL* curl;

  BinData data;
  CURLcode res;
  long resCode = 0;
  struct curl_slist* headers = NULL;

  unsigned short respCode = 0;
  data.mem = malloc(1);
  data.size = 0;

  curl = curl_easy_init();

  if(!curl) {
    fprintf(stderr, "curl init failed\n");

    return -1;
  }

  // SSL settings
  curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);

  // store response data to "data"
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToBuf);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void* ) &data);

  // set headers
  headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

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
        "https://oauth2.googleapis.com/token returned %d\n", resCode);
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
  struct json_object* json;
  json = json_tokener_parse(data.mem);

  // Check if correctly parsed
  if(json == NULL) {
    fprintf(stderr, "json-c parse failed\n");
    return -1;
  }
  // Copy obtained token to struct GOOGLE_AUTH* auth
  struct json_object* accessToken = json_object_object_get(json, "access_token");
  struct json_object* expiresIn = json_object_object_get(json, "expires_in");
  struct json_object* refreshToken = json_object_object_get(json, "refresh_token");
  struct json_object* refreshExpiresIn = json_object_object_get(json, "refresh_token_expires_in");
  struct json_object* scopeStrObj = json_object_object_get(json, "scope");
  time_t date_now = time(NULL);

  if(accessToken) {
    strcpy(auth ->token, json_object_get_string(accessToken));
    auth ->token_expire = date_now + (time_t)json_object_get_int64(expiresIn);
  }
  if(refreshToken) {
    strcpy(auth ->refresh, json_object_get_string(refreshToken));
    auth ->refresh_expire = date_now + (time_t)json_object_get_int64(refreshExpiresIn);
  }
  if(scopeStrObj) {
    parseScopes(auth, json_object_get_string(scopeStrObj));
  }
  // Clean up
  // Free received data
  free(data.mem);
  // Free json data
  json_object_put(json);

  return 0;
}

/*
 * Parses the query string returned by google and 
 * exchange the authorization code for access/refresh token.
 *
 * Params :
 * auth : auth ->token (and possibly auth ->refresh) will be assigned
 * queryStr : the query string from google
 * state : the state string you gave when you called createAuthUrl()
 */
int obtainTokenFromQuery(GOOGLE_AUTH* auth, char* queryStr, char* state) {
  // Parse query string
  // Buffers for parsing query string
  char authCode[257] = "";
  char scopeStr[1280] = "";
  char stateStr[512] = "";

  char* query_trim = queryStr;
  // Ignore leading "?"
  if(query_trim[0] == '?') query_trim++;

  for(short i = 0; ; i++) {
    char* dest;
    // ?name1=value1&name2=value2.
    if(strncmp("code=", query_trim + i, 5) == 0) {
      dest = authCode;
      i += 5;
    }else if(strncmp("scope=", query_trim + i, 6) == 0) {
      dest = scopeStr;
      i += 6;
    }else if(strncmp("state=", query_trim + i, 6) == 0) {
      dest = stateStr;
      i += 6;
    }else if(strncmp("error=", query_trim + i, 6) == 0) {
      // Got error response
      fprintf(stderr, "OAuth2 Error\n");

      return -1;
    }else {
      while(query_trim[i] && (query_trim[i] != '&')) i++;
      // Reached end of str
      if(!query_trim[i]) break;
      // read next
      else continue;
    }

    // Then read its value
    short dest_i = 0;
    for( ; query_trim[i] && (query_trim[i] != '&'); (i++ && dest_i++)) {
      if(query_trim[i] == '%') {
        // Decode url
        char ch = '\0';
        i++;
        ch = (query_trim[i] - 0x37 + (query_trim[i] >= '0' && query_trim[i] <= '9') * 7);
        i++;
        ch = ch * 16 + (query_trim[i] - 0x37 + (query_trim[i] >= '0' && query_trim[i] <= '9') * 7);

        dest[dest_i] = ch;
      }
      else dest[dest_i] = query_trim[i];
    }
    // Terminate the string
    dest[dest_i] = '\0';
    // Reached end of str
    if(!query_trim[i]) break;
  }

  // Check if state string matches
  if(strcmp(stateStr, state)) {
    fprintf(stderr, "state mismatch; possible CORS attack\n");
#ifdef GOOGLE_STRICT
    return -1;
#endif
  }
  // Parse scope string
  if(parseScopes(auth, scopeStr)) return -1;

  // generate POST data
  char postBody[1280] = "";
  char* authCode_encoded;
  char* redirect_encoded;
  // first, encode the values
  authCode_encoded = curl_easy_escape(NULL, authCode, strlen(authCode));
  redirect_encoded = curl_easy_escape(NULL, auth ->cred ->redirect, strlen(auth ->cred ->redirect));
  // then put them in the template
  sprintf(postBody, "code=%s&client_id=%s&client_secret=%s&redirect_uri=%s&grant_type=authorization_code",
      authCode_encoded, auth ->cred ->id, auth ->cred ->secret, redirect_encoded);
  // free encoded strings
  curl_free(authCode_encoded);
  curl_free(redirect_encoded);

  // Request access token
  return exchangeToken(auth, postBody);
}

/*
 * Use refresh token to get new access token.
 *
 * Params :
 * auth : auth ->token will be replaced
 */
int refreshAccessToken(GOOGLE_AUTH* auth) {
  // generate POST data
  char postBody[1280] = "";
  // then put them in the template
  sprintf(postBody, "client_id=%s&client_secret=%s&grant_type=refresh_token&refresh_token=%s",
      auth ->cred ->id, auth ->cred ->secret, auth ->refresh);

  // Request access token
  return exchangeToken(auth, postBody);
}
