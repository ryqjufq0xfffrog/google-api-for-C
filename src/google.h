typedef struct _ggl_slist {
  char str[256];
  struct _ggl_slist* next;
} GOOGLE_SLIST;

typedef struct {
  char id[256];
  char secret[256];
  char token[2049];
  char refresh[513];
  char redirect[2048];
  GOOGLE_SLIST* scopes;
  char state[512];
} GOOGLE_AUTH;

// Setup and Cleanup
void goog_global_init();
void goog_global_cleanup();

// Memory
void goog_free(void*);
void goog_free_auth(GOOGLE_AUTH*);

GOOGLE_SLIST* goog_slist_append(GOOGLE_SLIST*, char*);
void goog_list_free(GOOGLE_SLIST*);

// Utils
char* createAuthUrl(GOOGLE_AUTH*, GOOGLE_SLIST*);
int obtainTokenFromQuery(GOOGLE_AUTH*, char*);
