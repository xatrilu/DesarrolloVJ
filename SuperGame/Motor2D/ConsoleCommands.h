#ifndef _CONSOLECOMMANDS_H_
#define _CONSOLECOMMANDS_H_
#include "j1Module.h"

struct p2SString;
class j1Module;

#define MAX_COMMAND_LENGTH 32

class ConsoleCommands
{
public:
	ConsoleCommands(const char* g_command, j1Module* g_callback, uint min_arg, uint max_args, const char* info);
	~ConsoleCommands();

public:
	p2SString text;
	j1Module* callback;
	uint max_args;
	uint min_args;
	const char* info;
};

#endif