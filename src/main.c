#include <stdlib.h>
#include "shell.h"

int main(int argc, char* argv[]){
	shell *acsh = new_shell();
	routine(acsh);
	return EXIT_SUCCESS;
}
