typedef struct {
  char id[256];
  char secret[256];
  char token[2049];
  char refresh[513];
  char redirect[2048];
  long scopes[7];
  char state[512];
} GOOGLE_AUTH;

typedef struct {
  int mail: 1;
  int calendar: 1;

} GOOGLE_SCOPE;

// Setup and Cleanup
void goog_global_init();
void goog_global_cleanup();

// Utils
char* createAuthUrl(GOOGLE_AUTH*, char**);
int obtainTokenFromQuery(GOOGLE_AUTH*, char*);
