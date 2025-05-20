#ifndef ENGINE_H
#define ENGINE_H

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

/* getkeys: gets the provided keys from the provided id,
 * if the id is set to -1, it will get the provided key from every document,
 * if keys is NULL, it will get every key. */
tablist_t *getkeys(tablist_t *list, int id, char **keys, int len);

/* setkeys: sets the provided key-value pairs in the provided object,
 * if the id is -1, it will set the provided pairs in every object. */
int setkeys(tablist_t **list, int id, char **pairs, int len);

/* delkeys: deletes the provided keys from the provided id,
 * if the id is -1, it will delete the provided key from every document,
 * if keys is NULL it, will delete every key. */
int delkeys(tablist_t *list, int id, char **keys, int len);

// TODO: make readdb and writedb rely on helpers
// to make them both more private

// TODO: reimplement binary formatting, again

/* readdb: reads the provided db file,
 * if the filename is NULL, it will return an empty table list,
 * if the file format is invalid, it will return NULL. */
tablist_t *readdb(char *filename);

/* writedb: writes a table list to a database file. */
void writedb(char *filename, tablist_t *list);

#endif
