#include "Helpers.h"
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

struct sigaction custom_sigaction_constructor(int signal, void (*handler)(int), int flags, const char* symbolic_name){
	struct sigaction sig;

	sig.sa_handler = handler;
	if(	sigemptyset(&sig.sa_mask) == -1 ){
		printf("Error when emptying sa_mask field of %s multiple external background worker!", symbolic_name);
		perror("");
	}
	sig.sa_flags = flags;
	if(sigaction(signal, &sig, NULL) == -1){
		printf("Error when setting the signal handler of %s with sigaction!", symbolic_name);
		perror("");
	}

	return sig;
}

void wait_forall_children(int options){
	int status, pid;
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
