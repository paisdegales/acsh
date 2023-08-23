#pragma once

#include <stddef.h>

void prompt(const char* const path);

int read_stdin(char *buffer, const size_t size);

void routine();

int test_internal_cmd(char const* const executable);

void shell_cd(char **const cwd, char const* const path);
