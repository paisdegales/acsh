#include "ExecutionClassifier.h"
#include "Parser.h"
#include "Command.h"

int classify_commands_execution(commands* cmds){
	const size_t number_commands = get_number_commands(cmds);

	if(!number_commands){
		return NothingToRun;
	}

	//Checking for any errors associated with the parsing of the commandline read
	for(size_t i=0; i<number_commands; i++){
		if(!valid_command(get_command_commands(cmds, i))){
			return InvalidExecution;
		}
	}

	command* cmd;
	unsigned char running_mode;

	if(number_commands > 1){
		for(size_t i=0; i<number_commands; i++){
			cmd = get_command_commands(cmds, i);
			running_mode = get_running_mode_command(cmd);
			if(is_internal_command(cmd)){
				return MultipleInternalCommandsDisallowed;
			}
			if(running_mode){ // running_mode equal to 1 means it should be run in foreground
				return MultipleExternalCommandsForegroundDisallowed;
			}
		}
		return RunMultipleExternalCommandsBackground;
	}

	cmd = get_command_commands(cmds, 0);
	running_mode = get_running_mode_command(cmd);

	if(is_internal_command(cmd)){
		if(!running_mode){ // running_mode equal to 0 means it should be run in background

			return SingleInternalCommandBackgroundDisallowed;
		}
		return RunSingleInternalCommandForeground;
	}

	if(running_mode){
		return RunSingleExternalCommandForeground;
	}

	return RunSingleExternalCommandBackground;
}

char* describe_execution_classification(int exec_class){
	switch(exec_class){
		case RunMultipleExternalCommandsBackground:
			return "Run multiple external commands in background";
		case RunSingleExternalCommandBackground:
			return "Run a single external command in background";
		case RunSingleExternalCommandForeground:
			return "Run a single external command in foreground";
		case RunSingleInternalCommandForeground:
			return "Run a single internal command in foreground";
		case InvalidExecution:
			return "Invalid Execution; Something went wrong when parsing this command";
		case MultipleExternalCommandsForegroundDisallowed:
			return "Multiple external commands can only be run in background";
		case MultipleInternalCommandsDisallowed:
			return "Can't run multiple internal commands";
		case SingleInternalCommandBackgroundDisallowed:
			return "Can't run single internal command in background";
		case NothingToRun:
			return "Nothing to run";
		default:
			return "Unknown execution";
	}
}
