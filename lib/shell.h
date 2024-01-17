#pragma once

#include <stddef.h>
#include <signal.h>

typedef struct shell shell;

/* basic */
shell* new_shell(void);
shell* destroy_shell(shell* sh);
void routine(shell* const sh);

/* tasks */
void prompt(const char* const path);
int read_stdin(char *buffer, const size_t size);
void execute_commands(shell *const sh);
int test_internal_cmd(char const* const executable);

/* internal commands */
void shell_cd(char **const cwd, char const* const path);

/* session management */
void store_pid(unsigned *const children, const size_t size, const int pid);
void cleanup_pid(unsigned *const children, const size_t size);
void kill_remaining_processes(unsigned *const children, const size_t size);

/* signal handling */
void keyboard_sig_handler(int sig);
void sigusr1_handler(int sig);
void set_sigusr1_handler(void);
void shell_handlers(struct sigaction *const oldsigquit, struct sigaction *const oldsigint);

/* for multiple external commands running in background */
void many_background(shell *const sh);
void shell_many_background(shell *const sh, const int jobmanager_pid);
void jobmanager_many_background(shell *const sh);

/* for a single external command running in foreground */
void single_ext_foreground(shell *const sh);
void shell_single_ext_foreground(void);
void worker_single_ext_foreground(shell *const sh);

/* for a single external command running in background */
void single_ext_background(shell *const sh);
void shell_single_ext_background(shell *const sh, const int worker_pid);
void worker_single_ext_background(shell *const sh);
