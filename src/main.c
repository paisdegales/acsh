#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tests.h"

int main(int argc, char* argv[]){
	//tokenize_string_test("carlos daniel <3 pereira pederneiras ;");
	//stringfy_tokens_test("carlos daniel <3 pereira pederneiras ;");
	//group_tokens_test("carlos daniel <3 pereira pederneiras ;");
	//stringfy_token_group_test("carlos daniel <3 pereira pederneiras ;");
	//command_test("git add . <3 cd ~/ <3 echo 'oi' % <3 <3 <3");
	//parse_test("git add . <3 cd ~/ <3 echo 'oi' % <3 <3 <3");
	//run_external_command_test("ls <3 git --help <3 <3");
	//classify_commands_execution_test("git add . <3 cd ~/ <3 echo 'oi' % <3 <3 <3");
	//classify_commands_execution_test("git add . <3 cd ~/ <3 echo 'oi' <3 <3 <3");
	//classify_commands_execution_test("git add . %");
	run_external_command_test("git add . <3 cd ~/ <3 echo 'oi' % <3 <3 <3");

	return 0;
}
