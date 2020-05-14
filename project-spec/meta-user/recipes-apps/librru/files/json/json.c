#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <linux/kernel.h>

#include "json.h"
#include "rrulog.h"

#define JSMN_SUCCESS 0

static char *mapfile(const char *fn, size_t *size) {
	unsigned ps = sysconf(_SC_PAGESIZE);
	struct stat st;
	char *map = NULL;
	int err;
	int fd = open(fn, O_RDONLY);

	if (fd < 0 && fn) {
		LOG_ERROR("Error opening events file %s\n", fn);
	}

	if (fd < 0)
		return NULL;
	err = fstat(fd, &st);
	if (err < 0)
		goto out;
	*size = st.st_size;
	map = mmap(NULL, (st.st_size + ps - 1) & ~(ps - 1), PROT_READ | PROT_WRITE,
			MAP_PRIVATE, fd, 0);
	if (map == MAP_FAILED)
		map = NULL;
	out: close(fd);
	return map;
}

static void unmapfile(char *map, size_t size) {
	unsigned ps = sysconf(_SC_PAGESIZE);
	munmap(map, roundup(size, ps));
}

/*
 * Parse json file using jsmn. Return array of tokens,
 * and mapped file. Caller needs to free array.
 */
jsmntok_t *parse_json(const char *fn, char **map, size_t *size, int *len) {
	jsmn_parser parser;
	jsmntok_t *tokens;
	int res;
	unsigned sz;

	*map = mapfile(fn, size);
	if (!*map)
		return NULL;

	sz = *size * 16;
	tokens = malloc(sz);
	if (!tokens)
		goto error;
	jsmn_init(&parser);
	res = jsmn_parse(&parser, *map, *size, tokens, sz / sizeof(jsmntok_t));
	if (res < 0) {
		LOG_ERROR("json error\n");
		goto error_free;
	}
	if (len)
		*len = parser.toknext;
	return tokens;
	error_free: free(tokens);
	error: unmapfile(*map, *size);
	return NULL;
}

void free_json(char *map, size_t size, jsmntok_t *tokens) {
	free(tokens);
	unmapfile(map, size);
}

static int countchar(char *map, char c, int end) {
	int i;
	int count = 0;
	for (i = 0; i < end; i++)
		if (map[i] == c)
			count++;
	return count;
}

/* Return line number of a jsmn token */
int json_line(char *map, jsmntok_t *t) {
	return countchar(map, '\n', t->start) + 1;
}

static const char * const jsmn_types[] = { [JSMN_PRIMITIVE] = "primitive",
		[JSMN_ARRAY] = "array", [JSMN_OBJECT] = "object", [JSMN_STRING
				] = "string" };

#define LOOKUP(a, i) ((i) < (sizeof(a)/sizeof(*(a))) ? ((a)[i]) : "?")

/* Return type name of a jsmn token */
const char *json_name(jsmntok_t *t) {
	return LOOKUP(jsmn_types, t->type);
}

int json_len(jsmntok_t *t) {
	return t->end - t->start + 1;
}

/* Is string t equal to s? */
int json_streq(char *map, jsmntok_t *t, const char *s) {
	unsigned len = json_len(t);
	return len == strlen(s) && !strncasecmp(map + t->start, s, len);
}


int num_of_child (jsmntok_t *jsmntok, int len, int parent)
{
	int count = 0;
	int i = 0;

	for (i = parent + 1; i < len; i++)
	{
		if (jsmntok[i].parent == parent)
			count++;
	}
	return count;
}

int find_token(jsmntok_t *jsmntok, char *map, int len, const char *name){
    int i;
    int toklen;

    for (i = 0; i < len; i++){
        toklen = jsmntok[i].end - jsmntok[i].start;
        if (toklen != strlen(name)) continue;
        if (!strncmp(name, (map+jsmntok[i].start), strlen(name))){
            return i;
        }
    }
    return -1;
}

void get_string (char *js, jsmntok_t *jsmntok, char *val)
{
	strncpy (val, js+jsmntok->start, jsmntok->end - jsmntok->start);
	val[jsmntok->end - jsmntok->start] = '\0'; 			
}
void get_param_info (jsmntok_t *jsmntok, char *js, int len, char *node_name, int *num_of_el, int *num_of_node, int *parent)
{
	int pos;
	pos = find_token(jsmntok, js, len, node_name);
	if (jsmntok[pos+1].type == JSMN_ARRAY)
		pos = pos + 1;
	*parent = pos;
	*num_of_el = num_of_child (jsmntok, len, pos);
	*num_of_node = num_of_child (jsmntok, len, pos + 1);
}

void get_node_array (jsmntok_t *jsmntok, int parent, int len, int *pos)
{
	int i, j = 0;

	for (i = parent+1; i<len; i++)
	{
		if (jsmntok[i].parent == parent)
		{
			pos[j] = i;
			j++;
		}
	}
}

long find_long (jsmntok_t* jsmntok, char* js, int *pos, int size, char *key)
{
	int i;

	char val[100];

	for (i = 0; i < size; i++)
	{	
		strncpy (val, js + jsmntok[pos[i]].start, jsmntok[pos[i]].end - jsmntok[pos[i]].start);
		val[jsmntok[pos[i]].end - jsmntok[pos[i]].start] = '\0'; 
		
		if (!strcmp(key, val))
		{
			strncpy (val, js+jsmntok[pos[i] + 1].start, jsmntok[pos[i] + 1].end - jsmntok[pos[i] + 1].start);
			val[jsmntok[pos[i] + 1].end - jsmntok[pos[i] + 1].start] = '\0';
			return strtol (val, NULL, 0);
		}
	}
	LOG_ERROR("Node %s not found\n", key);

	return 0;
}

long find_long2 (jsmntok_t* jsmntok, char* js, int size, char *key)
{
	int i, *pos, tmp;

	char val[100];
	tmp = find_token(jsmntok, js, json_len(jsmntok), key);
	pos = &tmp;
	for (i = 0; i < size; i++)
	{	
		strncpy (val, js + jsmntok[pos[i]].start, jsmntok[pos[i]].end - jsmntok[pos[i]].start);
		val[jsmntok[pos[i]].end - jsmntok[pos[i]].start] = '\0'; 
		
		if (!strcmp(key, val))
		{
			strncpy (val, js+jsmntok[pos[i] + 1].start, jsmntok[pos[i] + 1].end - jsmntok[pos[i] + 1].start);
			val[jsmntok[pos[i] + 1].end - jsmntok[pos[i] + 1].start] = '\0';
			return strtol (val, NULL, 0);
		}
	}
	LOG_ERROR("Node %s not found\n", key);

	return 0;
}

int find_string (jsmntok_t* jsmntok, char* js, int size, char *key, char *val)
{
	int i, *pos, tmp;

	tmp = find_token(jsmntok, js, json_len(jsmntok), key);
	pos = &tmp;
	for (i = 0; i < size; i++)
	{	
		strncpy (val, js + jsmntok[pos[i]].start, jsmntok[pos[i]].end - jsmntok[pos[i]].start);
		val[jsmntok[pos[i]].end - jsmntok[pos[i]].start] = '\0'; 
		
		if (!strcmp(key, val))
		{
			strncpy (val, js+jsmntok[pos[i] + 1].start, jsmntok[pos[i] + 1].end - jsmntok[pos[i] + 1].start);
			val[jsmntok[pos[i] + 1].end - jsmntok[pos[i] + 1].start] = '\0';
			return 0;
		}
	}

	return -1;
}
