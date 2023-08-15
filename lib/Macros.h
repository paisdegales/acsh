#include <string.h>

#define create_local_copy(varname, str) char varname[strlen(str)+1]; strcpy(varname, str);
