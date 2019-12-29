#ifndef _MAIN_MENU_H_
#define _MAIN_MENU_H
#include "j1Module.h"

struct SDL_Texture;
class GuiImage;
class GuiText;
class GuiButton;
class GuiInputText;
enum class Menu;

class MainMenu : public j1Module
{
public:
	MainMenu();
	~MainMenu();

	bool Awake(pugi::xml_node& config);
	bool Start();
	bool PreUpdate() { return true; };
	bool Update(float dt) { return true; };
	bool PostUpdate();
	bool CleanUp() { return true; };

	void OnEvent(j1UI_Element* element, FocusEvent event);
	void CreateMainScreen();
	void CreateSettingsScreen();
	void CreateCreditsScreen();

private:
	Menu openMenu;
	int winWidth;
	int winHeight;
	bool fullscreen;
	SDL_Texture* BackgroundTex;
	SDL_Texture* TitleTex;
};

#endif 

