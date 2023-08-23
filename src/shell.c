#include "shell.h"
#include "tokens.h" //strtok_tokens, destroy_tokens, tokens_tokens, get_token, next_token, reverse_next_token
#include <errno.h> //ERRNO
#include <fcntl.h> // dup2, open, O_WRONLY
#include <signal.h> //SIGINT, SIGTERM, SIGQUIT, SIGTSTP, SA_RESTART, sigaction, struct sigaction, sigemptyset
#include <stdio.h> //printf, scanf, feof, ferror, perror, clearerr, getchar, stdin, stdout, stderr
#include <stdlib.h> //free
#include <string.h> //memset, strcmp
#include <sys/wait.h> //wait, WIFSIGNALED, WIFEXITED, WEXITSTATUS, WTERMSIG
#include <unistd.h> //chdir, kill, fork, setsid, execvp, exit

void prompt(const char* const path){
	printf("%s acsh> ", path);
}

int read_stdin(char *buffer, const size_t buffer_size){
	memset(buffer, 0, buffer_size);
	if(scanf("%[^\n]", buffer) <= 0){
		if(feof(stdin)){
			return 0;
		}
		else if(ferror(stdin)){
			perror("something went wrong when reading stdin");
			clearerr(stdin);
		}
		else{
			perror("something unexpected occurred!");
		}
		return -1;
	}
	getchar();
	return 1;
}

int test_internal_cmd(char const* const executable){
	if(!strcmp(executable, "cd")){
	   return 1;
	}
	else if(!strcmp(executable, "exit")){
		return 2;
	}
	return 0;
}

void shell_cd(char **const cwd, char const *const path){
	if(chdir(path) == -1){
		perror("acsh failed when changing dir");
	}
	if(*cwd){
		free(*cwd);
	}
	*cwd = getcwd(NULL, 0);
}

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

void routine(){
	char buffer[1000], *token, **tokens;
	token_it *job, *next_job;
	int status, pid;
	unsigned children[40];
	struct sigaction oldsigquit, oldsigint;
	char *path = getcwd(NULL, 0);

	shell_handlers(&oldsigquit, &oldsigint);
	memset(children, 0, 40*sizeof(unsigned));
	while(1){
		prompt(path);

		if(read_stdin(buffer, sizeof(buffer)) != 1){
			return ;
		}

		job = strtok_tokens(buffer, "<3");
		if(!job){
			continue;
		}

		next_job = strtok_tokens(NULL, "<3");
		if(next_job){
			/* 	if more than 1 job is found, the shell presumes
				that multiple external commands will run in the bg.
				No checking for the % operator is made therefore.  */
			//create jobManager process
			if((pid = fork())){
				//shell
				//store its job manager pid
				for(unsigned i=0; i<40; i++){
					if(!children[i]){
						children[i] = pid;
						break;
					}
				}
				//shell leaves and encharges the jobManager of creating all job processes
			}
			else{
				//job manager
				//set job manager's sid to a different session than the shell
				if(setsid() == -1){
					perror("Job manager has failed during setsid call");
				}
				//restore jobManager's signal handlers to the default
				if(sigaction(SIGQUIT, &oldsigquit, NULL) == -1){
					perror("Job manager failed when restoring SIGQUIT's handler");
				}
				if(sigaction(SIGINT, &oldsigint, NULL) == -1){
					perror("Job manager failed when restoring SIGINT's handler");
				}
				//add SIGUSR1 handler
				//create each job and put it in the background
				//child processes inherit file descriptors, thats neat!
				set_sigusr1_handler();
				int fd = open("/dev/null", O_WRONLY);
				if(fd == -1){
					perror("Job manager failed when opening /dev/null");
					break;
				}
				if(dup2(fd, 1) == -1){
					perror("Job manager failed when piping its stdout to /dev/null");
					break;
				}
				if(dup2(fd, 2) == -1){
					perror("Job manager failed when piping its stderr to /dev/null");
					break;
				}
				//iterate at least 5 times, one per available job
				for(int i=0; i<5 && job; i++){
					tokens = tokens_tokens(job);
					if(!fork()){
						//child
						if(execvp(tokens[0], tokens) == -1){
							perror("A child failed when executing an external program in bg");
						}
					}
					//job manager
					job = destroy_tokens(job);
					job = next_job;
					next_job = strtok_tokens(NULL, "<3");
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

		}
		else{
			/* 	if only 1 job is found, the shell has to take one step further
				and analyze if the job is an internal/external program
				and it's running mode.  */

			//test if token is external or internal
			token = get_token(job);
			status = test_internal_cmd(token);
			switch(status){
				case 1:
					//getting the dir target
					token = next_token(job);
					shell_cd(&path, token);
					break;
				case 2:
					//kill remaining processes
					for(unsigned i=0; i<40; i++){
						if(children[i]){
							if(kill(children[i], SIGTERM) == -1){
								perror("Acsh failed when killing a child!");
							}
						}
					}
					//deallocate all memory
					if(path){
						free(path);
					}
					exit(0);
				default:
					//external command
					// obtaining the last token
					token = reverse_next_token(job);
					if(!strcmp(token, "%")){
						//foreground

						if(fork()){
							//shell
							//the shell runs in bg and waits patiently for its child to terminate
							pid = wait(&status);
							if(WIFEXITED(status)){
								printf("External foreground command %d ended normally with status %d\n", pid, WEXITSTATUS(status));
							}
							else{
								printf("External foreground command %d was ended by signal %d\n", pid, WTERMSIG(status));
							}
						}
						else{
							//child
							if(sigaction(SIGQUIT, &oldsigquit, NULL) == -1){
								perror("Child failed when restoring SIGQUIT's handler");
							}
							if(sigaction(SIGINT, &oldsigint, NULL) == -1){
								perror("Child failed when restoring SIGINT's handler");
							}
							//remove % from the tokens
							tokens = tokens_tokens(job);
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
					}
					else{
						//background
						if((pid = fork())){
							//shell
							//store its child pid
							for(unsigned i=0; i<40; i++){
								if(!children[i]){
									children[i] = pid;
									break;
								}
							}
						}
						else{
							//child
							int fd = open("/dev/null", O_WRONLY);
							if(fd == -1){
								perror("Child failed when opening /dev/null");
								break;
							}
							else{
								if(dup2(fd, 1) == -1){
									perror("Child failed when piping its stdout to /dev/null");
									break;
								}
								if(dup2(fd, 2) == -1){
									perror("Child failed when piping its stderr to /dev/null");
									break;
								}
							}
							if(sigaction(SIGQUIT, &oldsigquit, NULL) == -1){
								perror("Child failed when restoring SIGQUIT's handler");
							}
							if(sigaction(SIGINT, &oldsigint, NULL) == -1){
								perror("Child failed when restoring SIGINT's handler");
							}
							tokens = tokens_tokens(job);
							if(execvp(tokens[0], tokens) == -1){
								perror("Child failed when executing external command in the fg");
							}
						}
					}
					break;
			}
			job = destroy_tokens(job);
		}

		//the shell checks if any jobManager/singled external process terminated. In such case,
		//it attempts to remove its pid from 'childs' array
		for(unsigned i=0; i<40; i++){
			if(children[i]){
				if(kill(children[i], 0) == -1){
					//an error occurred... this could mean that
					//this child has already terminated
					children[i] = 0;
					break;
				}
			}
		}
	}
}

