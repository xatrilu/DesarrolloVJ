#include "ConsoleCommands.h"
#include "p2SString.h"

ConsoleCommands::ConsoleCommands(const char* g_command, j1Module* g_callback, const char* g_explanation) {
	text = g_command;
	callback = g_callback;
	explanation = g_explanation;
}

ConsoleCommands::~ConsoleCommands() {}