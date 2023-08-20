#pragma once

#include <stddef.h>

/* stores all tokens of a sentence and how many of them there are */
typedef struct token_it token_it;

/* creates an iterator containing a vector of char*
 * each char* is a dynamic pointer to a token of the source string
 * the last char* of this iterator points to NULL
 * if no tokens are found, NULL is returned */
token_it* tokenize_string(const char *const source);

/* deallocate all memory allocated by tokens */
token_it* destroy_tokens(token_it* t);

/* prints the iterator */
void print_tokens(token_it const* const t);

/* checks if a tokens pointer is confidently 'usable', in which case the return value is 0 
 * such a pointer is considered 'usable' if it checks the following criteria:
 * 1. the pointer itself is not null 
 * 2. its internal vector of tokens is not null */
int empty_tokens(token_it const* const str);

/* glues all tokens found in 'tokens' into a single string using 'separator' as a separator */
char* stringfy_tokens(token_it const* const t, const char* separator);

/* gets the next token available in the iterator */
char* next_token(token_it* const t);

/* gets all available tokens in the iterator */
char** tokens_tokens(token_it const* const t);

size_t count_word(token_it * const tokens, char const* const word);

/* creates an token_it that stores all tokens of source up until encoutering the next occurrence of the delimiter */
token_it* strtok_tokens(char* const source, const char* const delimiter);

char* reverse_next_token(token_it *const t);

char* get_token(token_it const* const t);
