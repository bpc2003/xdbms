#ifndef CMD_H
#define CMD_H

enum cmdtype { GET, SET, DEL, ERR };

struct cmd {
	enum cmdtype type;
	char *selector;
	int plen;
	char **params;
};

struct cmd eval(char *str);

#endif
