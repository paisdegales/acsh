#pragma once
#include <signal.h>
#include <stddef.h>

int empty_string(char const * const str);

void wait_forall_children(int options);

char* strtok2(char *const str, char const*const delimiter);

size_t count_tokens(const char* source);
