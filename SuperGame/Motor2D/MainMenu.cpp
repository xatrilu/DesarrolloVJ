#include "MainMenu.h"
#include "j1Render.h"
#include "j1App.h"
#include "j1Gui.h"
#include "GuiButton.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Scene.h"
#include "j1Textures.h"
#include "Console.h"
#include "j1Window.h"
#include "j1Audio.h"
#include <Windows.h>
#include "j1Player.h"
#include "SDL/include/SDL.h"
#include "brofiler/Brofiler/Brofiler.h"

MainMenu::MainMenu() : j1Module()
{
	name.create("main_menu");
	fullscreen = false;
}

MainMenu::~MainMenu() {}

bool MainMenu::Awake(pugi::xml_node& config) 
{
	bool ret = true;
	_TTF_Font* font = App->font->Load("fonts/Lotterdam.ttf", 50);
	return ret;
}

bool MainMenu::Start()
{
	bool ret = true;

	App->gui->Start();

	winWidth = App->win->width;
	winWidth = App->win->height;

	App->audio->PlayMusic("MenuMusic.ogg");
	BackgroundTex = App->tex->Load("sprites/UI/MenuBackground.jpg");
	TitleTex = App->tex->Load("sprites/UI/Title.png");

	CreateMainScreen();
	return ret;
}

bool MainMenu::PostUpdate() 
{
	bool ret = true;
	return ret;
}

void MainMenu::OnEvent(j1UI_Element* element, FocusEvent event) 
{
	BROFILER_CATEGORY("Main Menu Events", Profiler::Color::CadetBlue)
	if ((element->type == UI_Type::BUTTON) && (event == FocusEvent::CLICKED))
	{
		GuiButton* button = (GuiButton*)element;
		switch (button->action)
		{
			LOG("action");
		case ButtonAction::PLAY:
			if (App->map->Load("Level1.tmx") == true)
			{
				int w, h;
				uchar* data = NULL;
				if (App->map->CreateWalkabilityMap(w, h, &data))
					App->pathfinding->SetMap(w, h, data);
				RELEASE_ARRAY(data);
			}

			App->render->camera.x = App->scene->startCameraPos.x;
			App->render->camera.y = App->scene->startCameraPos.y;

			App->gui->DestroyAllGui();
			if (App->console->visible) App->console->DestroyInterface();
			App->scene->CreateScreenUI();
			break;

		case ButtonAction::CONTINUE:
			App->gui->DestroyAllGui();
			App->LoadGame();
			App->scene->LevelChange(NO_MAP, LEVEL_1);
			App->scene->openMenu = Menu::NO_MENU;
			App->entities->blocked_movement = false;
			App->scene->CreateScreenUI();
			break;

		case ButtonAction::SETTINGS:
			App->gui->DestroyAllGui();
			CreateSettingsScreen();
			break;

		case ButtonAction::CREDITS:
			if (openMenu == Menu::MAIN_MENU) {
				App->gui->DestroyAllGui();
				CreateCreditsScreen();
				openMenu = Menu::CREDITS;
			}
			else ShellExecuteA(NULL, "open", "https://xatrilu.github.io/DesarrolloVJ/", NULL, NULL, SW_SHOWNORMAL);
			break;

		case ButtonAction::BACK:
			App->gui->DestroyAllGui();
			CreateMainScreen();
			break;

		case ButtonAction::ADRIA:
			if (openMenu == Menu::CREDITS) ShellExecuteA(NULL, "open", "https://github.com/Avilgor", NULL, NULL, SW_SHOWNORMAL);
				
			break;

		case ButtonAction::XAVIER:
			if (openMenu == Menu::CREDITS) ShellExecuteA(NULL, "open", "https://github.com/xatrilu", NULL, NULL, SW_SHOWNORMAL);

			break;
		case ButtonAction::FULLSCREEN:
			if (openMenu == Menu::SETTINGS) 
			{
				if (!fullscreen)
				{
					SDL_SetWindowFullscreen(App->win->window, SDL_WINDOW_FULLSCREEN);
					fullscreen = true;
				}
				else
				{
					SDL_SetWindowFullscreen(App->win->window, SDL_WINDOW_RESIZABLE);
					fullscreen = false;
				}
				winWidth = App->win->width;
				winWidth = App->win->height;
			}
			break;
		case ButtonAction::QUIT:
			App->quit = true;
			break;
		}
	}
}

void MainMenu::CreateMainScreen() 
{
	openMenu = Menu::MAIN_MENU;
	SDL_Rect camera;
	camera = App->render->camera;

	GuiImage* background = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this);
	background->Init({ 0,0 }, { 0,0,(int)App->win->width, (int)App->win->height });
	background->texture = BackgroundTex;

	GuiImage* title = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this);
	title->Init({ 100,100 }, { 0,0,800, 150 });
	title->texture = TitleTex;

	GuiButton* startBtn = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	startBtn->Init({ 400, 300 }, { 21,300,167,83 }, { 21,384,167,83 }, { 21,384,167,83 }, "Start", ButtonAction::PLAY);

	GuiButton* continueBtn = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	continueBtn->Init({ 400, 450 }, { 21,300,167,83 }, { 21,384,167,83 }, { 21,384,167,83 }, "Continue", ButtonAction::CONTINUE);

	GuiButton* settingsBtn = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	settingsBtn->Init({ 400, 600 }, { 21,300,167,83 }, { 21,384,167,83 }, { 21,384,167,83 }, "Settings", ButtonAction::SETTINGS);

	GuiButton* exitBtn = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	exitBtn->Init({ 50, 626 }, { 21,300,167,83 }, { 21,384,167,83 }, { 21,384,167,83 }, "Exit", ButtonAction::QUIT);

	GuiButton* creditsBtn = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	creditsBtn->Init({ 780, 626 }, { 21,300,167,83 }, { 21,384,167,83 }, { 21,384,167,83 }, "Credits", ButtonAction::CREDITS);
}

void MainMenu::CreateSettingsScreen() 
{
	openMenu = Menu::SETTINGS;
	GuiImage* background = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this);
	background->Init({ 0,0 }, { 0,0,(int)App->win->width,(int)App->win->height });
	background->texture = BackgroundTex;

	GuiImage* title = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this);
	title->Init({ 100, 80 }, { 0,0,800, 150 });
	title->texture = TitleTex;

	GuiImage* menuBackground = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this);
	menuBackground->Init({ 300, 415 }, { 192,327,517,76 });

	GuiButton* backBtn = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	backBtn->Init({ 20, 20 }, { 3,470,66,80 }, { 3,470,66,80 }, { 3,470,66,80 }, "", ButtonAction::BACK);

	GuiButton* fullscreenBtn = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	fullscreenBtn->Init({ 300,400 }, { 133, 470, 110,101 }, { 475, 477, 110,101 }, { 475,477,110,101 }, "", ButtonAction::FULLSCREEN, true);

	GuiText* fullscreenTxt = (GuiText*)App->gui->CreateUIElement(UI_Type::TEXT, this, nullptr, false, true);
	fullscreenTxt->Init({ 500,430 }, "Fullscreen");	
}

void MainMenu::CreateCreditsScreen()
{
	openMenu = Menu::CREDITS;
	GuiImage* background = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this);
	background->Init({ 0,0 }, { 0,0,(int)App->win->width,(int)App->win->height });
	background->texture = BackgroundTex;

	GuiImage* panel = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this);
	panel->Init({ 200,280 }, { 17,581,605,394 });

	GuiButton* backBtn = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	backBtn->Init({ 20, 20 }, { 3,470,66,80 }, { 3,470,66,80 }, { 3,470,66,80 }, "", ButtonAction::BACK);

	GuiImage* title = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this);
	title->Init({ 100, 80 }, { 0,0,800, 150 });
	title->texture = TitleTex;

	GuiButton* webpage = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	webpage->Init({ 420,520 }, { 21,300,167,83 }, { 21,384,167,83 }, { 21,384,167,83 }, "Web", ButtonAction::CREDITS);

	GuiButton* adria = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	adria->Init({ 270,390 }, { 21,300,167,83 }, { 21,384,167,83 }, { 21,384,167,83 }, "Adrià Avila", ButtonAction::ADRIA);

	GuiButton* xavier = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	xavier->Init({ 570,390 }, { 21,300,167,83 }, { 21,384,167,83 }, { 21,384,167,83 }, "Xavier Trillo", ButtonAction::XAVIER);
}