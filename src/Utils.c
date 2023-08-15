#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Macros.h"
#include "Language.h"
#include "Utils.h"

struct string_tokens{
	char **tokens;
	size_t number_tokens;
};

struct token_group{
	char ***groups;
	size_t number_groups;
};



/********************************* STRING_TOKENS FUNCTIONS **************************************/

string_tokens* tokenize_string(char* source){
	if(empty_string(source)){
		return NULL;
	}

	/* counting how many tokens there are in the string */
	create_local_copy(copy1, source);
	char *token = strtok(copy1, WORD_DELIMITER);
	size_t token_counter = 0;

	while(token){
		token_counter++;
		token = strtok(NULL, WORD_DELIMITER);
	}

	if(!token_counter){
		return NULL;
	}

	/* creating the vector of tokens and populating it */
	create_local_copy(copy2, source);
	char **tokens = calloc(token_counter, sizeof(char*));

	token = strtok(copy2, WORD_DELIMITER);
	for(size_t i=0; i<token_counter; i++){
		tokens[i] = strdup(token);
		token = strtok(NULL, WORD_DELIMITER);
	}

	string_tokens* str_tok = calloc(1, sizeof(string_tokens));
	str_tok->tokens = tokens;
	str_tok->number_tokens = token_counter;

	return str_tok;
}

string_tokens* destroy_string_tokens(string_tokens* tokens){
	if(!tokens){
		return NULL;
	}

	const size_t number_tokens = tokens->number_tokens;
	if(number_tokens){
		for(size_t i=0; i<number_tokens; i++){
			free(tokens->tokens[i]);
		}
		free(tokens->tokens);
	}
	free(tokens);

	return NULL;
}

void print_string_tokens(string_tokens* tokens){
	if(empty_string_tokens(tokens)){
		return ;
	}

	size_t const token_counter = tokens->number_tokens;
	for(size_t i=0; i<token_counter; i++){
		printf("token %lu = %s\n", i, tokens->tokens[i]);
	}
}

int empty_string_tokens(string_tokens* tokens){
	return (!tokens || !tokens->tokens) ? 1 : 0;
}

char* stringfy_tokens(string_tokens* tokens, const char* separator){
	if(empty_string_tokens(tokens)){
		return NULL;
	}

	/* counting the necessary number os chars to store all tokens separated by the chosen separator */
	size_t const number_tokens = tokens->number_tokens;
	size_t string_len = strlen(tokens->tokens[0]), separator_len = strlen(separator);
	for(size_t i=1; i<number_tokens; i++){
		string_len += separator_len + strlen(tokens->tokens[i]);
	}

	/* assembling the string */
	char* string = calloc(string_len+1, sizeof(char)); //+1 reason: null terminator
	strcat(string, tokens->tokens[0]);
	for(size_t i=1; i<number_tokens; i++){
		strcat(string, tokens->tokens[i]);
		strcat(string, separator);
	}

	return string;
}



/********************************* TOKEN_GROUP FUNCTIONS **************************************/

token_group* group_tokens(string_tokens* tokens, const char* separator){
	if(empty_string_tokens(tokens)){
		return NULL;
	}

	size_t const number_tokens = tokens->number_tokens;

	//each index of the array corresponds to a token of 'tokens'
	//1 for delimiter, 0 otherwise
	unsigned char is_delimiter[number_tokens];
	for(size_t i = 0; i<number_tokens; i++){
		is_delimiter[i] = !strcmp(tokens->tokens[i], separator) ? 1 : 0;
	}

	//counting how many groups there are in 'tokens' 
	//flip=1 and is_delimiter=0 : beginning of a group -> flip = 0
	//flip=0 and is_delimiter=1 : end of a group -> flip = 1
	//flip=0 and is_delimiter=0 : still in a group -> flip unchanged
	//flip 1 and is_delimiter=1 : repeated delimiter -> flip unchanged
	unsigned char flip = 1;
	size_t group_counter = 0;
	for(size_t i = 0; i<number_tokens; i++){
		if(flip && !is_delimiter[i]){
			group_counter++;
			flip = 0;
		}
		else if(!flip && is_delimiter[i]){
			flip = 1;
		}
	}

	//counting how many tokens each group has
	//each index of the array represents a group
	size_t number_tokens_group[group_counter];
	size_t token_counter = 0, j = 0;
	flip = 1;
	for(size_t i=0; i<number_tokens; i++){
		if(!is_delimiter[i]){
			token_counter++;
			flip = 0;
			if((i+1) == number_tokens){
				number_tokens_group[j] = token_counter;
			}
		}
		else if(!flip){
			number_tokens_group[j++] = token_counter;
			token_counter = 0;
			flip = 1;
		}
	}

	//make room for every token of each group
	char ***groups = calloc(group_counter, sizeof(char**));
	for(size_t i=0; i<group_counter; i++){
		groups[i] = calloc(number_tokens_group[i]+1, sizeof(char*)); //+1 meaning: NULL value means no more tokens for that group
	}

	//fill each group with its tokens
	size_t group_index = 0;
	token_counter = 0;
	for(size_t i = 0; i<number_tokens; i++){
		if(is_delimiter[i]){
			group_index++;
			token_counter = 0;
		}
		else{
			groups[group_index][token_counter++] = strdup(tokens->tokens[i]);
		}
	}

	token_group* tok_groups = calloc(1, sizeof(token_group));
	tok_groups->groups = groups;
	tok_groups->number_groups = group_counter;

	return tok_groups;
}

size_t get_number_groups(token_group* group){
	return group->number_groups;
}

char** get_group(token_group* group, size_t group_index){
	return group->groups[group_index];
}

token_group* destroy_token_group(token_group* group){
	if(!group){
		return NULL;
	}

	size_t const number_commands = group->number_groups;
	if(number_commands){
		for(size_t i=0; i<number_commands; i++){
			for(size_t j=0; group->groups[i][j] != NULL; j++){
				free(group->groups[i][j]);
			}
			free(group->groups[i]);
		}
		free(group->groups);
	}
	free(group);

	return NULL;
}

int empty_token_group(token_group* group){
	return (!group || !group->groups || !group->number_groups) ? 1 : 0;
}

void print_token_group(token_group* group){
	if(empty_token_group(group)){
		return ;
	}

	size_t const number_groups = group->number_groups;
	for(size_t i=0; i<number_groups; i++){
		printf("group %lu: ", i+1);
		for(size_t j=0; group->groups[i][j] != NULL; j++){
			printf("%s ", group->groups[i][j]);
		}
		putchar('\n');
	}
}

/* glues all tokens found in a group of 'groups' into a single string using 'separator' as a separator */
char* stringfy_token_group(token_group* groups, size_t group_index, const char* separator){
	if(empty_token_group(groups) || group_index >= groups->number_groups){
		return NULL;
	}

	/* counting the necessary number os chars to store all tokens separated by the chosen separator */
	char** tokens = groups->groups[group_index];
	size_t string_len = strlen(tokens[0]), separator_len = strlen(separator);
	for(size_t i=1; tokens[i]; i++){
		string_len += separator_len + strlen(tokens[i]);
	}

	/* assembling the string */
	char* string = calloc(string_len+1, sizeof(char)); //+1 reason: null terminator
	strcat(string, tokens[0]);
	for(size_t i=1; tokens[i]; i++){
		strcat(string, separator);
		strcat(string, tokens[i]);
	}

	return string;
}



/********************************* OTHER FUNCTIONS **************************************/

int empty_string(char * const str){
	if(!str){
		return 1;
	}

	size_t len = strlen(str);
	if(!len){
		return 1;
	}

	for(size_t i=0; i<len; i++){
		if(str[i] != ' ' || str[i] != '\n' || str[i] != '\v'){
			return 0;
		}
	}
	return 1;
}
