#pragma once

#include <signal.h>

struct sigaction custom_sigaction_constructor(int signal, void (*handler)(int), int flags, const char* symbolic_name);

void wait_forall_children(int options);
