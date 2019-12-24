#include "ConsoleCommands.h"
#include "p2SString.h"

ConsoleCommands::ConsoleCommands(const char* g_command, j1Module* g_callback, uint g_min_arg, uint g_max_args, const char* g_info) {
	text = g_command;
	callback = g_callback;
	min_args = g_min_arg;
	max_args = g_max_args;
	info = g_info;
}

ConsoleCommands::~ConsoleCommands() {}