#ifndef KEYTAB_H
#define KEYTAB_H

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
  struct keytab tab[TABLEN];
};

int *getkeys(struct keytablist *list, int id);
struct keytab getkey(struct keytablist *list, int id, char *key);
// TODO: rewrite setkey to take len as an argument
void setkey(struct keytablist *list, int id, char *pair);
void delkey(struct keytablist *list, int id, char *key);

#endif
