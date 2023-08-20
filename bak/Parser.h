#pragma once

#include "Command.h"

typedef struct commands commands;

commands* parse_commandline(char* cmdline_buffer);

commands* destroy_commands(commands* cmds);

size_t get_number_commands(commands* cmds);

command* get_command_commands(commands* cmds, size_t command_index);

command* remove_command_commands(commands* cmds, size_t command_index);

void print_commands(commands* cmds);
