#ifndef PARSER_H
#define PARSER_H

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

#endif
