#include "shell.h"
#include "tokens.h" //strtok_tokens, destroy_tokens, tokens_tokens, get_token, next_token, reverse_next_token
#include "macros.h" //STDIN_BUFFERSIZE, MAX_SESSIONS, MAX_JOBS, MAX_OPTIONS  
#include <errno.h> //ERRNO
#include <fcntl.h> //dup2, open, O_WRONLY
#include <signal.h> //SIGINT, SIGTERM, SIGQUIT, SIGTSTP, SA_RESTART, sigaction, struct sigaction, sigemptyset
#include <stdio.h> //printf, scanf, feof, ferror, perror, clearerr, getchar, stdin, stdout, stderr
#include <stdlib.h> //free, EXIT_SUCCESS, EXIT_FAILURE
#include <string.h> //memset, strcmp
#include <sys/wait.h> //wait, WIFSIGNALED, WIFEXITED, WEXITSTATUS, WTERMSIG
#include <unistd.h> //chdir, kill, fork, setsid, execvp, exit

struct shell{
	unsigned children[40];
	char *path;
	char buffer[STDIN_BUFFERSIZE];
	token_it *job, *next_job;
	struct sigaction oldsigquit, oldsigint;
};







/* basic */

shell* new_shell(){
	shell* sh = calloc(1, sizeof(shell));
	char *path = getcwd(NULL, 0);
	sh->path = path;
	shell_handlers(&sh->oldsigquit, &sh->oldsigint);
	return sh;
}

shell* destroy_shell(shell* sh){
	if(!sh){
		return NULL;
	}
	if(sh->path){
		free(sh->path);
	}
	free(sh);
	return NULL;
}

void routine(shell* const sh){
	int status;
	while(1){
		prompt(sh->path);

		status = read_stdin(sh->buffer, STDIN_BUFFERSIZE);
		if(status != 1){
			destroy_shell(sh);
			return ;
		}

		execute_commands(sh);

		//the shell checks if any jobManager/single external process terminated. In such case,
		//it attempts to remove its pid from 'childs' array
		cleanup_pid(sh->children, MAX_SESSIONS);
	}
}









/* tasks */

void prompt(const char* const path){
	printf("%s acsh> ", path);
}

int read_stdin(char *buffer, const size_t buffer_size){
	memset(buffer, 0, buffer_size);
	if(scanf("%[^\n]", buffer) <= 0){
		if(feof(stdin)){
			return 0;
		}
		if(ferror(stdin)){
			perror("something went wrong when reading stdin");
			clearerr(stdin);
			return -1;
		}
	}
	getchar();
	return 1;
}

int test_internal_cmd(char const* const executable){
	if(!strcmp(executable, CD_COMMAND)){
	   return CD_CMD_CODE;
	}
	if(!strcmp(executable, EXIT_COMMAND)){
		return EXIT_CMD_CODE;
	}
	return 0;
}

void execute_commands(shell *const sh){
	sh->job = strtok_tokens(sh->buffer, JOB_SEPARATOR);
	if(!sh->job){
		return ;
	}

	int status;
	char *token;

	sh->next_job = strtok_tokens(NULL, JOB_SEPARATOR);
	if(sh->next_job){
		/* 	if more than 1 job is found, the shell presumes
			that multiple external commands will run in the bg.
			No checking for the % operator is made therefore.  */
		many_background(sh);
		return ;
	}

	/* 	if only 1 job is found, the shell has to take one step further
		and analyze if the job is an internal/external program
		and it's running mode.  */

	//test if token is an external/internal command
	token = get_token(sh->job);
	status = test_internal_cmd(token);
	switch(status){
		case CD_CMD_CODE:
			//getting the dir target
			token = next_token(sh->job);
			shell_cd(&sh->path, token);
			break;
		case EXIT_CMD_CODE:
			kill_remaining_processes(sh->children, MAX_SESSIONS);
			destroy_shell(sh);
			exit(EXIT_SUCCESS);
		default:
			// external command
			// obtaining the last token
			token = reverse_next_token(sh->job);
			if(!strcmp(token, FG_OPERATOR)){
				single_ext_foreground(sh);
			}
			else{
				single_ext_background(sh);
			}
			break;
	}
	sh->job = destroy_tokens(sh->job);
}









/* internal commands */

void shell_cd(char **const cwd, char const *const path){
	if(chdir(path) == -1){
		perror("acsh failed when changing dir");
	}
	if(*cwd){
		free(*cwd);
	}
	*cwd = getcwd(NULL, 0);
}










/* signal handling */

void keyboard_sig_handler(int sig){
	if(sig == SIGINT){
		printf("Nao adianta me enviar o sinal por Ctrl-C... Estou vacinado!!\n");
	}
	else if(sig == SIGQUIT){
		printf("Nao adianta me enviar o sinal por Ctrl-\\... Estou vacinado!!\n");
	}
}

void sigusr1_handler(int sig){
	//	this handler attempts to kill all processes within the same GID
	if(kill(0, SIGTERM) == -1){
		perror("A process failed when attempting to kill all processes within its GID");
	}
}

void set_sigusr1_handler(){
	struct sigaction sigusr1;
	sigusr1.sa_flags = SA_RESTART;
	sigusr1.sa_handler = sigusr1_handler;
	if(sigemptyset(&sigusr1.sa_mask) == -1){
		perror("Job manager failed when emptying the signal mask for SIGUSR1's handler");
	}
	if(sigaction(SIGUSR1, &sigusr1, NULL) == -1){
		perror("Acsh failed when using sigaction to change SIGUSR1's handler");
	}
}

void shell_handlers(struct sigaction *const oldsigquit, struct sigaction *const oldsigint){
	struct sigaction sigquit;
	struct sigaction sigint;

	//make certain system calls (open, read..) restartable
	//after a call to this signal handler
	sigquit.sa_flags = SA_RESTART;
	sigquit.sa_handler = keyboard_sig_handler;

	//no signals should be blocked while sigquit handler is
	//happening. This was not specified in the pdf.
	if(sigemptyset(&sigquit.sa_mask) == -1){
		perror("Acsh failed when emptying the signal mask for SIGQUIT's handler");
	}
	if(sigaction(SIGQUIT, &sigquit, oldsigquit) == -1){
		perror("Acsh failed when using sigaction to change SIGQUIT's handler");
	}

	//make certain system calls (open, read..) restartable
	//after a call to this signal handler
	sigint.sa_flags = SA_RESTART;
	sigint.sa_handler = keyboard_sig_handler;

	//no signals should be blocked while sigint handler is
	//happening. This was not specified in the pdf.
	if(sigemptyset(&sigint.sa_mask) == -1){
		perror("Acsh failed when emptying the signal mask for SIGTSTP's handler");
	}
	if(sigaction(SIGINT, &sigint, oldsigint) == -1){
		perror("Acsh failed when using sigaction to change SIGTSTP's handler");
	}
}








/* session managing */

void store_pid(unsigned *const children, const size_t size, const int pid){
	for(unsigned i=0; i<size; i++){
		if(!children[i]){
			children[i] = pid;
			break;
		}
	}
}

void cleanup_pid(unsigned *const children, const size_t size){
	for(unsigned i=0; i<size; i++){
		if(children[i]){
			if(kill(children[i], 0) == -1){
				perror("Acsh failed during pid cleanup");
			}
			else{
				children[i] = 0;
			}
		}
	}
}

void kill_remaining_processes(unsigned *const children, const size_t size){
	for(unsigned i=0; i<size; i++){
		if(children[i]){
			if(kill(children[i], SIGTERM) == -1){
				perror("Acsh failed when killing a child process!");
			}
		}
	}
}










/* for multiple external commands running in background */

void many_background(shell * const sh){
	//create jobManager process
	int pid = fork();

	if(pid){
		shell_many_background(sh, pid);
	}
	else{
		jobmanager_many_background(sh);
	}
}

void jobmanager_many_background(shell *const sh){
	int pid, status;

	//set job manager's sid to a different session than the shell
	if(setsid() == -1){
		perror("Job manager has failed during setsid call");
	}
	//restore jobManager's signal handlers to the default
	if(sigaction(SIGQUIT, &sh->oldsigquit, NULL) == -1){
		perror("Job manager failed when restoring SIGQUIT's handler");
	}
	if(sigaction(SIGINT, &sh->oldsigint, NULL) == -1){
		perror("Job manager failed when restoring SIGINT's handler");
	}

	//add SIGUSR1 handler
	//child processes inherit file descriptors, thats neat!
	set_sigusr1_handler();
	int fd = open("/dev/null", O_WRONLY);
	if(fd == -1){
		perror("Job manager failed when opening /dev/null");
		return;
	}
	if(dup2(fd, 1) == -1){
		perror("Job manager failed when piping its stdout to /dev/null");
		return;
	}
	if(dup2(fd, 2) == -1){
		perror("Job manager failed when piping its stderr to /dev/null");
		return;
	}

	//create each job and put it in the background
	char **tokens;
	for(int i=0; i<MAX_JOBS && sh->job; i++){
		tokens = tokens_tokens(sh->job);
		if(!fork()){
			//child
			if(execvp(tokens[0], tokens) == -1){
				perror("A child failed when executing an external program in bg");
			}
		}
		//job manager
		sh->job = destroy_tokens(sh->job);
		sh->job = sh->next_job;
		sh->next_job = strtok_tokens(NULL, "<3");
	}

	//jobManager waits patiently for all its children to terminate
	while((pid = wait(&status)) != -1){
		if(WIFEXITED(status)){
			printf("External background command %d ended normally with status %d\n", pid, WEXITSTATUS(status));
		}
		else{
			printf("External background command %d was ended by signal %d\n", pid, WTERMSIG(status));
		}
	}
}

void shell_many_background(shell *const sh, const int jobmanager_pid){
	//store its job manager pid
	store_pid(sh->children, MAX_SESSIONS, jobmanager_pid);
	//shell leaves and encharges the jobManager of creating all job processes
}











/* for a single external command running in foreground */

void single_ext_foreground(shell *const sh){
	if(fork()){
		shell_single_ext_foreground();
	}
	else{
		worker_single_ext_foreground(sh);
	}
}

void shell_single_ext_foreground(){
	//the shell runs in bg and waits patiently for its child to terminate
	int pid, status;
	pid = wait(&status);
	if(WIFEXITED(status)){
		printf("External foreground command %d ended normally with status %d\n", pid, WEXITSTATUS(status));
	}
	else{
		printf("External foreground command %d was ended by signal %d\n", pid, WTERMSIG(status));
	}
}

void worker_single_ext_foreground(shell *const sh){
	char **tokens = tokens_tokens(sh->job);
	if(sigaction(SIGQUIT, &sh->oldsigquit, NULL) == -1){
		perror("Child failed when restoring SIGQUIT's handler");
	}
	if(sigaction(SIGINT, &sh->oldsigint, NULL) == -1){
		perror("Child failed when restoring SIGINT's handler");
	}

	//remove % from the tokens
	for(size_t i=0; tokens[i]; i++){
		if(!strcmp(tokens[i], "%")){
			tokens[i] = NULL;
			break;
		}
	}
	if(execvp(tokens[0], tokens) == -1){
		perror("Child failed when executing external command in the fg");
	}
}











/* for a single external command running in background */

void single_ext_background(shell *const sh){
	int pid = fork();
	if(pid){
		shell_single_ext_background(sh, pid);
	}
	else{
		worker_single_ext_background(sh);
	}
}

void shell_single_ext_background(shell *const sh, const int worker_pid){
	//store its worker pid
	store_pid(sh->children, MAX_SESSIONS, worker_pid);
}

void worker_single_ext_background(shell *const sh){
	int fd = open("/dev/null", O_WRONLY);
	char **tokens;
	if(fd == -1){
		perror("Child failed when opening /dev/null");
		return ;
	}
	else{
		if(dup2(fd, 1) == -1){
			perror("Child failed when piping its stdout to /dev/null");
			return ;
		}
		if(dup2(fd, 2) == -1){
			perror("Child failed when piping its stderr to /dev/null");
			return ;
		}
	}
	if(sigaction(SIGQUIT, &sh->oldsigquit, NULL) == -1){
		perror("Child failed when restoring SIGQUIT's handler");
	}
	if(sigaction(SIGINT, &sh->oldsigint, NULL) == -1){
		perror("Child failed when restoring SIGINT's handler");
	}
	tokens = tokens_tokens(sh->job);
	if(execvp(tokens[0], tokens) == -1){
		perror("Child failed when executing external command in the fg");
	}
}
