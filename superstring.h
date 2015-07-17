#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_FILE_LENGTH 255
#include <stdbool.h>
#include <math.h>
#define HASH_KEY_ALPHABET_LEN 1
#define HASH_ALPHA_LEN 26

#define HASH_BUCKETS 26
#define MAX_LEN_OF_STRING 256
#define MAX_NUM_OF_SUBSTRINGS 4012000
#define MAX_LEVELS_TO_DISPLAY 3
struct string_node_ {
	struct string_node_ *next;
	struct string_node_ *prev;
	int index;
	char *word;
	int key; /* string length */
	int num_of_substrings;
        struct sub_string_node_ *substring_nodes;
        struct string_node_ *next_super_node;
} typedef string_node_t;

struct string_hash_node_ {
	string_node_t *head;
	string_node_t *tail;
} typedef string_hash_node_t;

struct sub_string_node_ {
     string_node_t *node;
     struct sub_string_node_ *next;
} typedef sub_string_node_t;

string_hash_node_t string_hash_node[HASH_BUCKETS];
string_node_t *super_string_list = NULL;
sub_string_node_t sub_string_node_buffer[MAX_NUM_OF_SUBSTRINGS];
int sub_string_node_index;
bool enable_debug;

