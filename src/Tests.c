#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Tests.h"
#include "Language.h"
#include "Utils.h"
#include "Parser.h"
#include "Command.h"
#include "ExecutionClassifier.h"
#include "MultipleExternalBackgroundWorker.h"
#define begin_test(name) printf("\nTESTANDO A FUNCAO: %s\n", #name );
#define end_test(name) printf("FINALIZANDO TESTE DA FUNCAO: %s\n", #name );

void tokenize_string_test(char const* const string){
	begin_test(tokenize_string_test)

	char* input = strdup(string);

	string_tokens *tokens = tokenize_string(input);
	printf("String read: %s\ntokens:\n", input);
	print_string_tokens(tokens);

	destroy_string_tokens(tokens);
	free(input);

	end_test(tokenize_string_test)
}

void stringfy_tokens_test(char const* const string){
	begin_test(stringfy_tokens_test)

	char* input = strdup(string);

	string_tokens *tokens = tokenize_string(input);
	char* stringfy = stringfy_tokens(tokens, WORD_DELIMITER);

	printf("String read: %s\ntokens:\n", input);
	print_string_tokens(tokens);
	printf("\nTokens converted to a single string again:\n%s\n", stringfy);

	free(stringfy);
	destroy_string_tokens(tokens);
	free(input);

	end_test(stringfy_tokens_test)
}

void group_tokens_test(char const* const string){
	begin_test(group_tokens_test)

	char* input = strdup(string);

	string_tokens *tokens = tokenize_string(input);

	printf("String read: %s\ntokens:\n", input);
	print_string_tokens(tokens);

	token_group* group = group_tokens(tokens, COMMAND_DELIMITER);
	print_token_group(group);

	destroy_token_group(group);
	destroy_string_tokens(tokens);
	free(input);

	end_test(group_tokens_test)
}

void stringfy_token_group_test(char const* const string){
	begin_test(stringfy_token_group_test)

	char* input = strdup(string);

	string_tokens *tokens = tokenize_string(input);
	token_group* grp = group_tokens(tokens, COMMAND_DELIMITER);
	char* stringfy = stringfy_token_group(grp, 0, WORD_DELIMITER);

	printf("String read: %s\ntokens:\n", input);
	print_token_group(grp);
	printf("\nTokens converted to a single string again:\n%s\n", stringfy);

	free(stringfy);
	destroy_token_group(grp);
	destroy_string_tokens(tokens);
	free(input);

	end_test(stringfy_token_group_test)
}

void command_test(char const* const string){
	begin_test(command_test);

	char* input = strdup(string);

	string_tokens *tokens = tokenize_string(input);

	printf("String read: %s\ntokens:\n", input);
	print_string_tokens(tokens);

	token_group* group = group_tokens(tokens, COMMAND_DELIMITER);
	print_token_group(group);

	size_t number_groups = get_number_groups(group);
	command* cmd[number_groups];
	for(size_t i=0; i<number_groups; i++){
		cmd[i] = create_command_from_token_group(group, i);
		print_command(cmd[i]);
		destroy_command(cmd[i]);
	}
	destroy_token_group(group);
	destroy_string_tokens(tokens);
	free(input);

	end_test(command_test);
}

void parse_test(char* const string){
	begin_test(parse_test);

	commands* cmds = parse_commandline(string);
	print_commands(cmds);
	destroy_commands(cmds);

	end_test(parse_test);
}

void run_external_command_test(char* const string){
	begin_test(run_external_command_test);

	commands* cmds = parse_commandline(string);
	size_t number_commands = get_number_commands(cmds);

	for(size_t i=0; i<number_commands; i++){
		printf("Command about to be run:\n");
		print_command(get_command_commands(cmds, i));
		putchar('\n');

		if(fork() == 0){
			command* cmd = get_command_commands(cmds, i);
			run_external_command(cmd);
		}
	}

	destroy_commands(cmds);

	end_test(run_external_command_test);
}

void classify_commands_execution_test(char* const string){
	begin_test(classify_commands_execution_test);

	commands* cmds = parse_commandline(string);
	int classification;

	classification = classify_commands_execution(cmds);
	printf("execution classification generated: %s\n", describe_execution_classification(classification));

	destroy_commands(cmds);

	end_test(classify_commands_execution_test);
}

void run_multiple_external_commands_test(char * const str){
	begin_test(run_multiple_external_commands_test);

	commands* cmds = parse_commandline(str);

	run_multiple_external_commands_background(cmds);

	destroy_commands(cmds);

	end_test(run_multiple_external_commands_test);
}
