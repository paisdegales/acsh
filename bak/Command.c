#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "Command.h"
#include "Language.h"

enum command_type{
	external_cmd,
	internal_cmd,
	invalid_cmd
};

struct command{
	char *string, *executable, **options, **tokens;
	size_t string_len, number_options;
	unsigned char running_mode; //0 for background, 1 for foreground
	unsigned char type; //0 for external, 1 for internal, 2 for invalid
};

command* create_command_from_token_group(token_group* group, size_t group_index){
	if(empty_token_group(group) || group_index >= get_number_groups(group)){
		return NULL;
	}

	char** tokens = get_group(group, group_index);
	command* cmd = calloc(1, sizeof(command));

	/* the first string is treated as the executable and the following are options */
	char* executable = tokens[0];
	int type = tell_executable_type(executable);
	cmd->type = type;

	if(type == invalid_cmd){
		return cmd;
	}
	cmd->executable = strdup(executable);

	/* counting how many options there are in this group */
	size_t number_options = 0;
	for(size_t i=1; tokens[i] != NULL; i++, number_options++);

	if(number_options){
		//filling the options field
		cmd->options = calloc(number_options, sizeof(char*));
		cmd->number_options = number_options;
		for(size_t i=1; tokens[i] != NULL; i++){
			cmd->options[i-1] = strdup(tokens[i]);
		}

		//identifying the running mode of this command
		for(size_t i=0; i<number_options; i++){
			if(!strcmp(cmd->options[i], FG_OPERATOR)){ //if the token equals the foreground marker
				if((i+1) == number_options){ // if the fg marker is the last token of the command
					cmd->running_mode = 1;
				}
				else{
					cmd->type = invalid_cmd; //if the fg marker was found and is not the last word -> invalid syntax
				}
			}
		}
	}

	// +1 (executable)
	// +1 (null terminator)
	cmd->tokens = calloc(number_options+2, sizeof(char*));
	cmd->tokens[0] = cmd->executable;
	for(size_t i=0; i<number_options; i++){
		cmd->tokens[i+1] = cmd->options[i];
	}

	cmd->string = stringfy_token_group(group, group_index, WORD_DELIMITER);
	cmd->string_len = strlen(cmd->string);

	return cmd;
}

command* destroy_command(command* cmd){
	if(!valid_command(cmd)){
		return NULL;
	}

	if(cmd->number_options){
		for(size_t i=0; i<cmd->number_options; i++){
			free(cmd->options[i]);
		}
		free(cmd->options);
	}

	free(cmd->tokens);
	free(cmd->executable);
	free(cmd->string);
	free(cmd);

	return NULL;
}

int tell_executable_type(char* executable){
	if(!strcmp(executable, CD_OPERATION) || !strcmp(executable, EXIT_OPERATION)){
		return internal_cmd;
	}
	else if(!strcmp(executable, FG_OPERATOR) || !strcmp(executable, COMMAND_DELIMITER)){
		return invalid_cmd;
	}
	return external_cmd;
}

int get_running_mode_command(command* cmd){
	return cmd->running_mode;
}

int get_type_command(command* cmd){
	return cmd->type;
}

void print_command(command* cmd){
	if(!valid_command(cmd)){
		printf("Invalid command provided to print function\n");
		return ;
	}

	printf("Command type: %s\n", cmd->type == external_cmd? "External" : (cmd->type == internal_cmd ? "Internal" : "Invalid"));

	if(cmd->type == invalid_cmd){
		return ;
	}

	printf("Entire command: %s\n", cmd->string);
	printf("Executable: %s\n", cmd->executable);
	printf("Options (%lu): ", cmd->number_options);
	for(size_t i=0; i<cmd->number_options; i++){
		printf("%s ", cmd->options[i]);
	}
	printf("\nRunning mode: %s\n", !cmd->running_mode ? "Background" : "Foreground");
}

int valid_command(command* cmd){
	if(!cmd || cmd->type == invalid_cmd){
		return 0;
	}
	return 1;
}

void run_external_command(command* cmd){
	if(cmd->type != external_cmd){
		return ;
	}

	if(empty_string(cmd->string)){
		return ;
	}

	// +1 (executable)
	// +1 (null terminator)
	if(execvp(cmd->tokens[0], cmd->tokens) == -1){
		printf("Error when running command '%s'", cmd->string);
		perror("");
	}
}

int is_internal_command(command* cmd){
	if(!cmd || cmd->type != invalid_cmd){
		return 0;
	}
	return 1;
}
