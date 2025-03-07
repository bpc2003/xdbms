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

int *getkeys(struct keytab *tab);
struct keytab getkey(struct keytab *tab, char *key);
void setkey(struct keytab *tab, char *pair);
void delkey(struct keytab *tab, char *key);

#endif
