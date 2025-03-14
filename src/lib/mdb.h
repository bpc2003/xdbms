#ifndef MDB_H
#define MDB_H

#define TABLEN 1024

union value {
  char *str;
  double num;
  unsigned int b : 1;
};

struct keytab {
  char *key;
  int flag;
  union value v;
};

struct keytablist {
  int len;
  struct keytab tab[TABLEN];
};

// Table operations
int *getkeys(struct keytablist *list, int id);
struct keytab getkey(struct keytablist *list, int id, char *key);
int setkey(struct keytablist **list, int id, char *pair);
void delkey(struct keytablist *list, int id, char *key);

// file operations
struct keytablist *readdb(char *filename);
void writedb(char *filename, struct keytablist *list);

#endif
