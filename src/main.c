#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tokens.h"
#include "utils.h"
#include "test.h"
#include "shell.h"
//#include "Tests.h"

void* func(void* num1, void *num2, void* res){
	*(int*) res = *(int*)num1 + *(int*)num2;
	return res;
}

int compare(void* num1, void* num2){
	return *(int*)num1 == *(int*)num2;
}

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
	//run_external_command_test("git add . <3 cd ~/ <3 echo 'oi' % <3 <3 <3");
	
	//char str[] = "<3<3<<3";
	//char *token = strtok2(str, "<3");

	//while(token){
	//	printf("%s\n", token);
	//	token = strtok2(NULL, "<3");
	//}

	//int a=1, b=2, c, exp=3;
	//test3((void*)&a, (void*)&b, (void*)&c, (void*)&exp, func, compare);

	routine();

	return 0;
}
