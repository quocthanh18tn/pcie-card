#ifndef JSON_H
#define JSON_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "jsmn.h"

jsmntok_t *parse_json(const char *fn, char **map, size_t *size, int *len);
void free_json(char *map, size_t size, jsmntok_t *tokens);
int json_line(char *map, jsmntok_t *t);
const char *json_name(jsmntok_t *t);
int json_streq(char *map, jsmntok_t *t, const char *s);
int json_len(jsmntok_t *t);

int num_of_child (jsmntok_t *jsmntok, int len, int parent);
int find_token(jsmntok_t *jsmntok, char *map, int len, const char *name);
long find_long (jsmntok_t *jsmntok, char *js, int *pos, int size, char *key);
long find_long2 (jsmntok_t* jsmntok, char* js, int size, char *key);
int find_string (jsmntok_t *jsmntok, char *js, int size, char *key, char *val);
void get_param_info (jsmntok_t *jsmntok, char *js, int len, char *node_name, int *num_of_el, int *num_of_node, int *parent);
void get_node_array (jsmntok_t *jsmntok, int parent, int len, int *pos);
void get_string (char *js, jsmntok_t *jsmntok, char *val);

#ifndef bool
typedef unsigned int bool;
#endif

#ifndef true
#define	true 1
#endif

extern int eprintf(int level, int var, const char *fmt, ...);
#define pr_fmt(fmt)	fmt


#ifndef roundup
#define roundup(x, y) (                                \
{                                                      \
        const typeof(y) __y = y;                       \
        (((x) + (__y - 1)) / __y) * __y;               \
}                                                      \
)
#endif

#endif
