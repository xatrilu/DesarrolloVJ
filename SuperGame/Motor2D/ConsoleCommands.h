#ifndef _CONSOLECOMMANDS_H_
#define _CONSOLECOMMANDS_H_
#include "j1Module.h"

struct p2SString;
class j1Module;

#define MAX_COMMAND_LENGTH 32

class ConsoleCommands
{
public:
	ConsoleCommands(const char* command, j1Module* callback, const char* info);
	~ConsoleCommands();
	bool CleanUp();

public:
	p2SString text;
	j1Module* callback;
	const char* info;
};

#endif // !_j1COMMAND_H_	
