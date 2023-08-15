#pragma once

#include "Utils.h"

typedef struct command command;

/* creates a command using the ith group in 'group' */
command* create_command_from_token_group(token_group* group, size_t group_index);

command* destroy_command(command* cmd);

/* returns 0 in case executable is an external command, 1 in case its an internal command, or 2 if it's an invalid command */
int tell_executable_type(char* executable);

int get_running_mode_command(command* cmd);

int get_type_command(command* cmd);

void print_command(command* cmd);

/* returns 1 in case cmd is not null nor is an invalid command */
int valid_command(command* cmd);

void run_external_command(command* cmd);

int is_internal_command(command* cmd);
