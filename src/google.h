typedef struct {
  char id[256];
  char secret[256];
  char token[2049];
  char refresh[513];
  char redirect[2048];
  char** scopes;
} GOOGLE_AUTH;

// Setup and Cleanup
void goog_global_init();
void goog_global_cleanup();

// Utils
char* createAuthUrl(GOOGLE_AUTH*, char*);
int obtainTokenFromCode(GOOGLE_AUTH*, char*);
