#ifndef FILEOPS_H
#define FILEOPS_H

#define TABLEN 1024

extern int blen;

enum btype {
  BEGIN = 1,
  END, PAIR,

  ERROR
};

struct byte {
  enum btype type;
  char *value;
};

struct byte *parse(unsigned char *buf);

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
  struct keytab tab[TABLEN];
};

int *getkeys(struct keytablist *list, int id);
struct keytab getkey(struct keytablist *list, int id, char *key);
void setkey(struct keytablist **list, int *len, int id, char *pair);
void delkey(struct keytablist *list, int id, char *key);

// TODO: integrate every header into single file
// TODO: make readdb return struct keytab list*
unsigned char *readdb(char *filename);
void writedb(char *filename, struct keytablist *list, int len);

#endif
