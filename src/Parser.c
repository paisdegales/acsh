#include <stdio.h>
#include <stdlib.h>
#include "Parser.h"
#include "Utils.h"
#include "Language.h"

struct commands{
	command** cmds;
	size_t number_commands;
};
 
commands* parse_commandline(char* cmdline_buffer){
	string_tokens* tokens = tokenize_string(cmdline_buffer);
	token_group* grp = group_tokens(tokens, COMMAND_DELIMITER);

	size_t number_commands = get_number_groups(grp);
	command** cmds = calloc(number_commands, sizeof(command*));
	for(size_t i=0; i<number_commands; i++){
		cmds[i] = create_command_from_token_group(grp, i);
	}

	commands* c = calloc(1, sizeof(commands));
	c->cmds = cmds;
	c->number_commands = number_commands;

	destroy_token_group(grp);
	destroy_string_tokens(tokens);

	return c;
}

commands* destroy_commands(commands* cmds){
	if(cmds->number_commands){
		for(size_t i=0; i<cmds->number_commands; i++){
			destroy_command(cmds->cmds[i]);
		}
		free(cmds->cmds);
	}
	free(cmds);
	return NULL;
}

size_t get_number_commands(commands* cmds){
	return cmds->number_commands;
}

command* get_command_commands(commands* cmds, size_t command_index){
	return cmds->cmds[command_index];
}

command* remove_command_commands(commands* cmds, size_t command_index){
	command* cmd = cmds->cmds[command_index];
	cmds->cmds[command_index] = NULL;
	return cmd;
}

void print_commands(commands* cmds){
	for(size_t i=0; i<cmds->number_commands; i++){
		print_command(cmds->cmds[i]);
		putchar('\n');
	}
}
