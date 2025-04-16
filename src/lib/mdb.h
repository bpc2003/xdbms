#ifndef MDB_H
#define MDB_H

#define TABLEN 1024

typedef struct {
  char *key;
  int flag;
  union {
    char *str;
    double num;
    unsigned int boolean : 1;
  } value;
} tabidx_t;

typedef struct {
  int len;
  tabidx_t tab[TABLEN];
} tablist_t;

// Table operations
// TODO: Make this private
tabidx_t getkey(tablist_t *list, int id, char *key);

// Batch Operations
tabidx_t *getkeys(tablist_t *list, int id, char **keys, int len);  // TODO: Reimplement getkeys
int setkeys(tablist_t **list, int id, char **pairs, int len);
int delkeys(tablist_t *list, int id, char **keys, int len);

// file operations
tablist_t *readdb(char *filename);
void writedb(char *filename, tablist_t *list);

#endif
