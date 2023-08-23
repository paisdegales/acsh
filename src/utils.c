#include <assert.h> //assert
#include <stdio.h> //printf, perror
#include <string.h> //strlen, strtok, strcpy, memset
#include <sys/wait.h> //waitpid, WIFEXITED, WIFSTOPPED, WIFSIGNALED, WIFCONTINUED, WSTATUS, WSTOPSIG, WTERMSIG
#include "macros.h" //WHITE_CHARACTERS
#include "utils.h"

int empty_string(const char * const str){
	if(!str){
		return 1;
	}

	const size_t len = strlen(str);
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

void wait_forall_children(int const options){
	int status, pid;
	//wait(&options) is equivalent to waitpid(-1, &status, 0)
	while((pid = waitpid(-1, &status, options)) != -1 ){
		if(WIFEXITED(status)){
			printf("Child with %d pid has ended normally with status code %d\n", pid, WEXITSTATUS(status));
			perror("Errno info");
		}
		else if(WIFSTOPPED(status)){
			printf("Child with %d pid was stopped by signal %d\n", pid, WSTOPSIG(status));
			perror("Errno info");
		}
		else if(WIFCONTINUED(status)){
			printf("Child with %d pid was resumed by delivery of SIGCONT\n", pid);
		}
		else if(WIFSIGNALED(status)){
			printf("Child with %d pid was terminated by signal %d\n", pid, WTERMSIG(status));
			perror("Errno info");
		}
		else{
			printf("Something unexpected happened with child whose pid is %d... perhaps a core dump?\n", pid);
			perror("Errno info");
		}
	}
}

char* strtok2(char *const str, char const *const delimiter){
	if(empty_string(delimiter)){
		return NULL;
	}

	static char * reference = NULL;
	if(str){
		reference = str;
	}
	if(!reference){
		return NULL;
	}

	size_t const delim_size = strlen(delimiter);
	size_t const size = strlen(reference);

	if(delim_size > size){
		return NULL;
	}

	char *token = reference;
	size_t const limit = size - delim_size;
	for(size_t i=0; i<=limit; i++){
		if(!strncmp(reference+i, delimiter, delim_size)){
			memset(reference+i, 0, delim_size);
			reference += (i+delim_size);
			return token;
		}
	}

	//nothing was found
	reference = NULL;

	return token;
}

size_t count_tokens(const char* source){
	assert(source);

	char copy[strlen(source) + 1];
	strcpy(copy, source);

	char *token = strtok(copy, WHITE_CHARACTERS);
	size_t token_counter = 0;
	while(token){
		token_counter++;
		token = strtok(NULL, WHITE_CHARACTERS);
	}

	return token_counter;
}

