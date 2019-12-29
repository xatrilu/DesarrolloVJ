#include "ConsoleCommands.h"
#include "p2SString.h"

ConsoleCommands::ConsoleCommands(const char* command, j1Module* callback, const char* inf) {
	text = command;
	callback = callback;
	info = inf;
}

ConsoleCommands::~ConsoleCommands() {}