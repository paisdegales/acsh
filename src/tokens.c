#include <stdio.h> //printf, perror
#include <stdlib.h> //calloc, free
#include <string.h> //strlen, strtok, strcpy, strcat
#include <assert.h> //assert
#include "macros.h" //WHITE_CHARACTERS
#include "utils.h" //strtok2, count_tokens
#include "tokens.h"


struct token_it{
	char **tokens;
	size_t size, index;
};


token_it* tokenize_string(const char* const source){
	if(empty_string(source)){
		return NULL;
	}

	/* +1: the last token is a NULL pointer */
	const size_t token_counter = count_tokens(source) + 1;
	char copy[strlen(source)];
	strcpy(copy, source);

	char **tokens = calloc(token_counter, sizeof(char*));
	char* token = strtok(copy, WHITE_CHARACTERS);
	size_t i = 0;
	while(token){
		tokens[i++] = strdup(token);
		token = strtok(NULL, WHITE_CHARACTERS);
	}

	token_it* str_tok = calloc(1, sizeof(token_it));
	str_tok->tokens = tokens;
	str_tok->size = token_counter;

	return str_tok;
}


token_it* destroy_tokens(token_it* t){
	if(!t){
		return NULL;
	}

	/*
	t->size always has at least one element (the terminating NULL
	pointer). By checking if it is greater than 1, we're implicitly
	checking if there are tokens dinamically stored inside t->tokens
	*/
	if(t->size > 1){
		const size_t size = t->size - 1;
		for(size_t i=0; i<size; i++){
			free(t->tokens[i]);
		}
	}
	free(t->tokens);
	free(t);

	return NULL;
}


void print_tokens(token_it const* const t){
	if(empty_tokens(t)){
		return ;
	}

	size_t const token_counter = t->size;
	printf("(%lu)\n", token_counter);

	for(size_t i=0; i<token_counter; i++){
		printf("token %lu = %s\n", i, t->tokens[i]);
	}
}


int empty_tokens(token_it const* const t){
	return (!t || !t->tokens) ? 1 : 0;
}


char* stringfy_tokens(token_it const* const t, const char* separator){
	if(empty_tokens(t)){
		return NULL;
	}

	/* counting the necessary number of chars to store all tokens separated by the chosen separator */
	size_t const size = t->size;
	size_t string_len = strlen(t->tokens[0]), separator_len = strlen(separator);
	for(size_t i=1; i<size; i++){
		string_len += separator_len + strlen(t->tokens[i]);
	}

	/* assembling the string */
	char* string = calloc(string_len+1, sizeof(char)); //+1 reason: null terminator
	strcat(string, t->tokens[0]);
	for(size_t i=1; i<size; i++){
		strcat(string, t->tokens[i]);
		strcat(string, separator);
	}

	return string;
}


char* next_token(token_it* const t){
	t->index++;
	t->index %= t->size;
	return t->tokens[t->index];
}


char** tokens_tokens(token_it const* const t){
	return t->tokens;
}


size_t count_word(token_it * const tokens, char const* const word){
	char *token = next_token(tokens);
	size_t count = 0;
	while(token){
		if(!strcmp(token, word)){
			count++;
		}
		token = next_token(tokens);
	}
	return count;
}

token_it* strtok_tokens(char* const source, const char* const delimiter){
	static char * reference = NULL;
	char *token;
	if(source){
		reference = source;
		token = strtok2(reference, delimiter);
	}
	else{
		token = strtok2(NULL, delimiter);
	}
	token_it* t = tokenize_string(token);
	return t;
}

char* reverse_next_token(token_it *const t){
	if(t->size == 1){
		return NULL;
	}

	if(t->index){
		t->index -= 1;
	}
	else{
		t->index = t->size - 2;
	}

	return t->tokens[t->index];
}

char* get_token(token_it const* const t){
	return t->tokens[t->index];
}
