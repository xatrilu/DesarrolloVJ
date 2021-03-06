#include "Console.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Gui.h"
#include "ConsoleCommands.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "brofiler/Brofiler/Brofiler.h"

Console::Console() : j1Module() 
{
	visible = false;
	CleanUpStarted = false;
	command_input = nullptr;
	currentCommand = nullptr;
}

Console::~Console() {}

bool Console::Awake(pugi::xml_node& config) 
{
	bool ret = true;

	return ret;
}

bool Console::Start()
{
	bool ret = true;
	AddLogText("Console started");

	CreateCommand("list", (j1Module*)this, "List all console commands");
	App->console->CreateCommand("quit", (j1Module*)this, "Quit the game");
	App->console->CreateCommand("fps_", (j1Module*)this, "Change FPS cap");

	return ret;
}

bool Console::PreUpdate()
{
	bool ret = true;
	return ret;
}

bool Console::Update(float dt)
{
	BROFILER_CATEGORY("ConsoleUpdate", Profiler::Color::MediumPurple)
		bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN)
	{
		if (visible)
		{
			DestroyInterface();
		}
		else
		{
			CreateInterface();
		}
	}

	if (visible) 
	{
		if ((App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN))
		{
			DestroyInterface();
		}

		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			p2SString input_text(App->input->text);
			App->console->AddLogText(input_text);
			command_input->GetText()->text.Clear();

			input_commands.add(input_text);

			CheckCommand(input_text);
		}

		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN) 
		{
			if (input_commands.end != nullptr)
			{
				if (currentCommand == nullptr)
				{
					currentCommand = input_commands.end;
					command_input->GetText()->text = currentCommand->data;
				}
				else
				{
					if (currentCommand->prev != nullptr) {
						currentCommand = currentCommand->prev;
						command_input->GetText()->text = currentCommand->data;
					}
				}
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN) 
		{
			if (input_commands.end != nullptr)
			{
				if (currentCommand == nullptr)
				{
					currentCommand = input_commands.end;
					command_input->GetText()->text = currentCommand->data;
				}
				else
				{
					if (currentCommand->next != nullptr) {
						currentCommand = currentCommand->next;
						command_input->GetText()->text = currentCommand->data;
					}
				}
			}
		}

		if (command_input != nullptr)command_input->Input();
	}
	return ret;
}

bool Console::PostUpdate() 
{
	BROFILER_CATEGORY("GuiPostUpdate", Profiler::Color::DarkViolet)
		bool ret = true;

	if (visible)
	{
		//Draw console
		App->render->DrawQuad(LOGBox, 0, 0, 0, 255);
		App->render->DrawQuad(command_background, 0, 0, 255, 255);
		command_input->Draw();

		for (p2List_item<GuiText*>* item = screenLOG.start; item != nullptr; item = item->next)
		{
			item->data->Draw();
		}
	}
	return ret;
}

bool Console::CleanUp()
{
	bool ret = true;
	CleanUpStarted = true;

	for (p2List_item<GuiText*>* item = screenLOG.start; item != nullptr; item = item->next)
	{
		App->gui->DestroyUIElement(item->data);
		screenLOG.del(item);
	}

	return ret;
}

void Console::CreateInterface() 
{
	App->input->EnableTextInput(true);
	iPoint camera = { App->render->camera.x, App->render->camera.y };
	LOGBox = { -camera.x, -camera.y, (int)App->win->width, 350 };
	command_background = { -camera.x, LOGBox.h - camera.y, (int)App->win->width, 40 };

	command_input = (GuiInputText*)App->gui->CreateUIElement(UI_Type::INPUT_TEXT, this, nullptr, false, true);
	command_input->Init({ 10, LOGBox.h }, "Write Command", { 0,(int)(LOGBox.y + LOGBox.h),(int)App->win->width, command_background.h }, false, CONSOLE_FONT);
	command_input->rect = command_background;

	App->gui->focused_element = command_input;
	command_input->HandleFocusEvent(FocusEvent::FOCUS_IN);

	for (p2List_item<p2SString>* item = log_record.start; item != nullptr; item = item->next)
	{
		GuiText* log_text = new GuiText(this);
		if (item->prev == nullptr)
		{
			log_text->Init({ 20,10 - App->render->camera.y }, item->data, CONSOLE_FONT);
		}
		else
		{
			log_text->Init({ 20 - App->render->camera.x, (int)(screenLOG.end->data->screenPos.y + screenLOG.end->data->rect.h) }, item->data, CONSOLE_FONT);
		}
		screenLOG.add(log_text);
	}

	visible = true;
	App->entities->blocked_movement = true;
}

void Console::DestroyInterface() {
	if (command_input != nullptr) App->gui->DestroyUIElement(command_input);

	for (p2List_item<GuiText*>* item = screenLOG.start; item != nullptr; item = item->next)
	{
		item->data->text = nullptr;
	}

	visible = false;
	App->entities->blocked_movement = false;
	App->input->EnableTextInput(false);
}

void Console::AddLogText(p2SString new_text) {
	if (!CleanUpStarted)
	{
		if (log_record.count() > 16)
		{
			log_record.del(log_record.start);
		}
		log_record.add(new_text);
		if (visible)
		{
			p2List_item<p2SString>* item2 = log_record.end;
			for (p2List_item<GuiText*>* item = screenLOG.end; item != nullptr; item = item->prev)
			{
				item->data->text = item2->data;
			}
		}
	}
}

void Console::CreateCommand(const char* g_command, j1Module* g_callback, const char* info) {
	ConsoleCommands* command = new ConsoleCommands(g_command, g_callback, info);
	commands.add(command);
}

void Console::CheckCommand(p2SString command) {
	char given_initial_three[5] = "0000";
	char listed_initial_three[5] = "0000";
	char* given_command = (char*)command.GetString();


	for (p2List_item<ConsoleCommands*>* item = commands.start; item != nullptr; item = item->next)
	{
		if (item->data->text == command)
		{
			item->data->callback->Commands(command);
			return;
		}

		for (int i = 0; i < 4; i++)
		{
			given_initial_three[i] = command.GetString()[i];
			listed_initial_three[i] = item->data->text.GetString()[i];
		}

		if (strcmp(given_initial_three, listed_initial_three) == 0)
		{
			item->data->callback->Commands(command);
			return;
		}
	}

	LOG("Invalid command");
}

void Console::Commands(p2SString command) {
	char fps_string[8] = "fps_120";
	char initial_three[5] = "0000";

	if (command == "list")
	{
		for (p2List_item<ConsoleCommands*>* item = commands.start; item != nullptr; item = item->next)
		{
			char string[200];
			strcpy_s(string, item->data->text.GetString());
			strcat_s(string, " : ");
			strcat_s(string, item->data->info);
			AddLogText(string);
		}
		return;
	}

	if (command == "quit") {
		App->quit = true;
		return;
	}

	for (int i = 0; i < 3; i++)
	{
		initial_three[i] = command.lowercased().GetString()[i];
	}

	if (strcmp(initial_three, "fps") == 0);
	{
		int fps = 0;
		if (command.Length() == 7)
		{
			char fps_char[3];
			fps_char[0] = command.GetString()[4];
			fps_char[1] = command.GetString()[5];
			fps_char[2] = command.GetString()[6];

			int units = (int)fps_char[2] - 48;
			int dozens = (int)fps_char[1] - 48;
			int hundreds = (int)fps_char[0] - 48;
			fps = units + dozens * 10 + hundreds * 100;
		}
		else if (command.Length() == 6)
		{
			char fps_char[2];
			fps_char[0] = command.GetString()[4];
			fps_char[1] = command.GetString()[5];
			int units = (int)fps_char[1] - 48;
			int dozens = (int)fps_char[0] - 48;
			fps = units + dozens * 10;
		}
		else
		{
			LOG("Not valid fps cap number");
			return;
		}

		if (fps > 120 || fps < 30) 
		{			
			LOG("Set a number between 30 and 120");
		}
		else
		{
			App->framerate_cap = fps;
			LOG("Framerate cap changed to: %i", fps);
		}
	}
}