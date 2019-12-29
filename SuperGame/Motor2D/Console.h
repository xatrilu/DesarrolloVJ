#ifndef _CONSOLE_H_
#define _CONSOLE_H_
#include "j1Module.h"
#include "GuiText.h"
#include "SDL_image/include/SDL_image.h"

class GuiInputText;
struct SDL_Rect;
class ConsoleCommands;

#define MAX_LOG_RECORD 150

class Console : public j1Module
{
public:
	Console();
	~Console();

	bool Awake(pugi::xml_node& config);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	void Commands(p2SString command);
	void AddLogText(p2SString new_text);
	void CreateCommand(const char* command, j1Module* callback, const char* info = "No explanation given");
	void CheckCommand(p2SString command);
	void CreateInterface();
	void DestroyInterface();


public:
	GuiInputText* command_input;

	p2List<p2SString> log_record;
	p2List<GuiText*> screenLOG;
	p2List<p2SString> input_commands;
	p2List<ConsoleCommands*> commands;
	p2List_item<p2SString>* currentCommand;

	SDL_Rect LOGBox;
	SDL_Rect command_background;
	bool visible;
	bool CleanUpStarted;
};

#endif 

