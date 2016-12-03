#include "commandine.h"

#include <Windows.h>
#include <stdlib.h>

char g_cmdLinePath[260];
char g_cmdLineArgs[260];

void populate_cmdline() {

	char *raw = GetCommandLine();

	char *start;

	if (raw[0] == '"') {
		start = &raw[1]; // skip first character
	} else {
		start = &raw[0]; // complex expression to see the difference to above
	}

	char drive[3];
	char dir[256];
	char filename[256];
	char ext[256];


	_splitpath_s(start, drive, dir, filename, ext);

	if (ext[strlen(ext) - 2] = '"') {
		ext[strlen(ext) - 2] = 0;
	}
}

inline char *get_cmdline_exe() {
	return g_cmdLinePath;
}

inline char *get_cmdline_args() {
	return g_cmdLineArgs;
}