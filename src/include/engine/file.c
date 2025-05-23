#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine.h"

static const uint32_t ND_MARK = 0xffffffff;
static const uint32_t V_MARK = 0xfefffffe;

static const char *HEAD = "XDBF";

static int check(char **known, int len, char *str);
static void freeknown(char ***known, int len);

static long getsz(FILE *fp);
static int checkmark(char *buf, int pos);
static char *readidx(char *buf, int *i, char ***known, int *len, int *pos);

tablist_t *readdb(char *filename) {
	FILE *fp;
	int len = 2;
	tablist_t *list = calloc(len, sizeof(tablist_t));
	list[0].len = len;
	if (filename == NULL || (fp = fopen(filename, "rb")) == NULL)
		return list;
	long sz = getsz(fp);
	char *buf = calloc(sz, sizeof(char));
	fread(buf, sizeof(char), sz, fp);
	fclose(fp);

	if (strncmp(buf, HEAD, 4)) {
		free(buf);
		free(list);
		return NULL;
	}

	int tlen, vlen, t = 0, v = 0, l = -1;
	char **ktags = calloc((tlen = 2), sizeof(char *));
	char **kvalues = calloc((vlen = 2), sizeof(char *));
	char *key, *value;
	for (int i = 4; i < sz; ++i) {
		if (checkmark(buf, i) == 2) {
			i += 4;
			value = readidx(buf, &i, &kvalues, &vlen, &v);
			char *pair = calloc(strlen(key) + strlen(value) + 2, sizeof(char));
			sprintf(pair, "%s:%s", key, value);
			setkeys(&list, l, &pair, 1);
			free(pair);
			key = value = NULL;
		} else {
			key = readidx(buf, &i, &ktags, &tlen, &t);
			if (!strcmp(key, "document"))
				++l;
		}
	}
	freeknown(&ktags, t);
	freeknown(&kvalues, v);
	free(buf);
	return list;
}

static char *readidx(char *buf, int *i, char ***known, int *len, int *pos) {
	char *r = NULL;
	if (checkmark(buf, *i) == 1) {
		if (*len >= *pos) {
			*len *= 2;
			*known = realloc(*known, *len * sizeof(char *));
		}
		*i += 4;
		int nlen;
		memcpy(&nlen, buf + *i, sizeof(int));
		*i += sizeof(int);
		(*known)[*pos] = calloc(nlen + 1, sizeof(char));
		strncpy((*known)[(*pos)++], buf + *i, nlen);
		r = (*known)[(*pos) - 1];
		*i += nlen - 1;
	} else {
		int idx;
		memcpy(&idx, buf + *i, sizeof(int));
		*i += sizeof(int) - 1;
		if (idx > *pos)
			return NULL;
		r = (*known)[idx];
	}
	return r;
}

static int checkmark(char *buf, int pos) {
	if (!memcmp(buf + pos, &ND_MARK, sizeof(uint32_t)))
		return 1;
	else if (!memcmp(buf + pos, &V_MARK, sizeof(uint32_t)))
		return 2;
	return 0;
}

static long getsz(FILE *fp) {
	fseek(fp, 0, SEEK_END);
	long sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return sz;
}

static void newtag(char *tag, FILE *fp);
static void writeidx(char ***known, int *len, int *pos, char *str, FILE *fp);

void writedb(char *filename, tablist_t *list) {
	FILE *fp = fopen(filename, "wb");
	if (fp == NULL)
		return;
	fwrite(HEAD, sizeof(char), 4, fp);
	int tlen, vlen, t = 0, v = 0;
	char **ktags = calloc((tlen = 2), sizeof(char *));
	writeidx(&ktags, &tlen, &t, "documents", fp);
	writeidx(&ktags, &tlen, &t, "document", fp);

	char buf[64];
	char **kvalues = calloc((vlen = 2), sizeof(char *));
	tablist_t *indexes = getkeys(list, -1, NULL, 0);
	for (int i = 0; i < indexes[0].len; ++i) {
		if (i > 0)
			writeidx(&ktags, &tlen, &t, "document", fp);
		for (int j = 0; indexes[i].tab[j].flag; ++j) {
			writeidx(&ktags, &tlen, &t, indexes[i].tab[j].key, fp);
			fwrite(&V_MARK, sizeof(uint32_t), 1, fp);
			switch (indexes[i].tab[j].flag) {
			case 1:
				snprintf(buf, 64, "%g", indexes[i].tab[j].value.num);
				writeidx(&kvalues, &vlen, &v, buf, fp);
				break;
			case 2:
				writeidx(&kvalues, &vlen, &v,
						 indexes[i].tab[j].value.boolean ? "true" : "false",
						 fp);
				break;
			case 3:
				writeidx(&kvalues, &vlen, &v, indexes[i].tab[j].value.str, fp);
				break;
			}
		}
	}
	freeknown(&ktags, t);
	freeknown(&kvalues, v);
	free(indexes);
	fclose(fp);
}

static void newtag(char *tag, FILE *fp) {
	int len = strlen(tag);
	fwrite(&ND_MARK, sizeof(uint32_t), 1, fp);
	fwrite(&len, sizeof(int), 1, fp);
	fwrite(tag, sizeof(char), len, fp);
}

static void writeidx(char ***known, int *len, int *pos, char *str, FILE *fp) {
	int n;
	if ((n = check(*known, *pos, str)) >= 0)
		fwrite(&n, sizeof(int), 1, fp);
	else {
		if (*pos >= *len) {
			*len *= 2;
			*known = realloc(*known, *len * sizeof(char *));
		}
		(*known)[*pos] = calloc(strlen(str) + 1, sizeof(char));
		strcpy((*known)[*pos], str);
		newtag((*known)[(*pos)++], fp);
	}
}

static void freeknown(char ***known, int pos) {
	for (int i = 0; i < pos; ++i)
		free((*known)[i]);
	free(*known);
}

static int check(char **known, int len, char *str) {
	for (int i = 0; i < len; ++i) {
		if (!strcmp(str, known[i]))
			return i;
	}
	return -1;
}
