#pragma once

#include "Parser.h"

enum execution_classification{
	RunMultipleExternalCommandsBackground,
	RunSingleExternalCommandBackground,
	RunSingleExternalCommandForeground,
	RunSingleInternalCommandForeground,
	InvalidExecution,
	MultipleExternalCommandsForegroundDisallowed,
	MultipleInternalCommandsDisallowed,
	SingleInternalCommandBackgroundDisallowed,
	NothingToRun
};

int classify_commands_execution(commands*);

char* describe_execution_classification(int exec_class);
