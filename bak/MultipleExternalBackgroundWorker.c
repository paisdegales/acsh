#include "ExecutionClassifier.h"
#include "Helpers.h"
#include "Command.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

static void sigusr1_handler(int);

int run_multiple_external_commands_background(commands* cmds){
	pid_t worker_manager_pid = fork();

	if(worker_manager_pid){
		return worker_manager_pid;
	}

	if(setsid() == -1){
		perror("Error in multiple external commands background func: setting the new session id for worker_manager has failed");
	}

	custom_sigaction_constructor(SIGINT, SIG_IGN, SA_RESTART, "SIGINT");
	custom_sigaction_constructor(SIGTSTP, SIG_IGN, SA_RESTART, "SIGTSTP");
	custom_sigaction_constructor(SIGQUIT, SIG_IGN, SA_RESTART, "SIGQUIT");
	custom_sigaction_constructor(SIGUSR1, sigusr1_handler, SA_RESTART, "SIGUSR1");

	const size_t number_commands = get_number_commands(cmds);
	size_t command_index;
	int worker_pid;

	for(command_index = 0; command_index<number_commands; command_index++){
		worker_pid = fork();
		if(!worker_pid){
			break;
		}
	}

	if(!worker_pid){ //workers
		command* cmd = get_command_commands(cmds, command_index);
		run_external_command(cmd);
	}
	else{ //worker_manager
		wait_forall_children(0);
	}

	return 0;
}

static void sigusr1_handler(int signal){
	printf("SIGUSR1_HANDLER IN MULTIPLE EXTERNAL BACKGROUND WORKER IS WAITING FOR ITS IMPLEMENTATION!\n");

	// if the process handling SIGUSR1 happens to be the worker_manager
	if(getpid() == getppid()){
		//kill all its children by calling kill...
		return ;
	}

	//send to worker_manager SIGUSR1 by calling kill...
}
