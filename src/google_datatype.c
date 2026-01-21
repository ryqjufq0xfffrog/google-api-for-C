#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "./google.h"

/*
 * Call this method before calling any of google methods
 */
void goog_global_init() {
  curl_global_init(CURL_GLOBAL_DEFAULT);

  return;
}

/*
 * Everything's done? Then call this method.
 */
void goog_global_cleanup() {
  curl_global_cleanup();
  
  return;
}

/*
 * free memory allocated by malloc
 */
void goog_free(void* ptr) {
  free(ptr);
  
  return;
}

/*
 * append the string at the end of the given GOOGLE_SLIST
 * returns the new list.
 * (use like list = goog_slist_append(list, "blah");)
 * Params:
 * list : a list of string
 * str : the string to append at the end of the list
 */
GOOGLE_SLIST* goog_slist_append(GOOGLE_SLIST* list, char* str) {
  GOOGLE_SLIST* newItem = (GOOGLE_SLIST* ) malloc(sizeof(GOOGLE_SLIST));
  strcpy(newItem ->str, str);
  newItem ->next = list;
  
  return newItem;
}

/*
 * free a GOOGLE_SLIST
 */
void goog_list_free_all(GOOGLE_SLIST* list) {
  while(list) {
    GOOGLE_SLIST* prev = list;
    list = list ->next;
    goog_free(prev);
  }
  
  return;
}

/*
 * GOOGLE_CRED constructor
 */
GOOGLE_CRED* new_GoogleCred() {
  GOOGLE_CRED* cred = (GOOGLE_CRED* ) malloc(sizeof(GOOGLE_CRED));
  
  return cred;
}

/*
 * GOOGLE_AUTH constructor
 */
GOOGLE_AUTH* new_GoogleAuth(GOOGLE_CRED* cred) {
  GOOGLE_AUTH* auth = (GOOGLE_AUTH* ) malloc(sizeof(GOOGLE_AUTH));
  auth ->cred = cred;
  auth ->scopes = NULL;
  
  return auth;
}

/*
 * free a GOOGLE_AUTH
 */
void goog_free_auth(GOOGLE_AUTH* auth) {
  goog_list_free_all(auth ->scopes);
  goog_free(auth);
}

