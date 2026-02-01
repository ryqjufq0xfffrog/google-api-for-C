typedef struct _ggl_slist {
  char str[256];
  struct _ggl_slist* next;
} GOOGLE_SLIST;

typedef struct {
  char id[256];
  char secret[256];
  GOOGLE_SLIST* redirect;
} GOOGLE_CRED;

typedef struct {
  GOOGLE_CRED* cred;
  char token[2049];
  time_t token_expire;
  char refresh[513];
  time_t refresh_expire;
  GOOGLE_SLIST* scopes;
} GOOGLE_AUTH;

// Setup and Cleanup
void goog_global_init();
void goog_global_cleanup();

// Memory
void goog_free(void*);

void goog_free_auth(GOOGLE_AUTH*);
GOOGLE_AUTH* new_GoogleAuth(GOOGLE_CRED*);
GOOGLE_CRED* new_GoogleCred();

GOOGLE_SLIST* goog_slist_append(GOOGLE_SLIST*, const char*);
void goog_list_free_all(GOOGLE_SLIST*);

// Utils
GOOGLE_CRED* readCredentialsFromFile(char*);
char* createAuthUrl(GOOGLE_AUTH*, GOOGLE_SLIST*, char*, char*, int, char*);
int obtainTokenFromQuery(GOOGLE_AUTH*, char*, char*, char*);
