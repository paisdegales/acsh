#pragma once
#include <stddef.h>

/* stores all tokens of a sentence and how many of them there are */
typedef struct string_tokens string_tokens;

/* stores groups of tokens and how many of them there are */
typedef struct token_group token_group;

/********************************* STRING_TOKENS FUNCTIONS **************************************/

/* creates a vector of tokens of a source string using spaces as the delimiter 
 * if no tokens are found, NULL is returned
 * if at least one token is found, the internal vector of tokens is allocated with a size >= 1 
 * the tokens stored in the internal vector are dinamically allocated strings and they are never null nor spaces 
 * '\n' and '\v' are treated as tokens, but they can later on be removed */
string_tokens* tokenize_string(char * const source);

/* correctly deallocate all memory allocated by string_tokens */
string_tokens* destroy_string_tokens(string_tokens* tokens);

void print_string_tokens(string_tokens* tokens);

/* checks if a string_tokens pointer is confidently 'usable', in which case the return value is 0 
 * such a pointer is considered as 'usable' if it checks the following criteria:
 * 1. the pointer itself is not null 
 * 2. its internal vector of tokens is not null */
int empty_string_tokens(string_tokens* str);

/* glues all tokens found in 'tokens' into a single string using 'separator' as a separator */
char* stringfy_tokens(string_tokens* tokens, const char* separator);



/********************************* TOKEN_GROUP FUNCTIONS **************************************/

/* groups tokens using a separator */
token_group* group_tokens(string_tokens* tokens, const char* delimiter);

size_t get_number_groups(token_group* group);

char** get_group(token_group* group, size_t group_index);

/* correctly deallocate all memory allocated token_group */
token_group* destroy_token_group(token_group* group);

/* checks if a token_group pointer is confidently 'usable', in which case the return value is 0 (antonym: empty)
 * such a pointer is considered as 'usable' if it checks the following criteria:
 * 1. the pointer itself is not null 
 * 2. its internal vector of groups is not null
 * 3. its internal count of how many groups there are is not 0 */
int empty_token_group(token_group* group);

void print_token_group(token_group* group);

/* glues all tokens found in a group of 'groups' into a single string using 'separator' as a separator */
char* stringfy_token_group(token_group* groups, size_t group_index, const char* separator);



/********************************* OTHER FUNCTIONS **************************************/

/* checks whether or not a string has only blank characters, that is: ' ', '\n' or '\v' */
int empty_string(char * const str);

