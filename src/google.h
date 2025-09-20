typedef struct {
  char id[256];
  char secret[256];
} CREDENTIALS;

char* createAuthUrl(char* id, char** scopes, char* state, char* redirect);
