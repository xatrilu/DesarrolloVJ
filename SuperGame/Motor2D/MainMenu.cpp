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
#include "SDL/include/SDL.h"

MainMenu::MainMenu() : j1Module()
{
	name.create("main_menu");
	fullscreen = false;
}

MainMenu::~MainMenu() {}

bool MainMenu::Awake(pugi::xml_node& config) {
	bool ret = true;

	return ret;
}

bool MainMenu::Start() {
	bool ret = true;

	App->gui->Start();

	window_width = App->win->width;
	window_width = App->win->height;

	CreateMainScreen();
	//App->audio->PlayMusic("path_to_follow.ogg");
	visible_menu = Menu::MAIN_MENU;
	return ret;
}

bool MainMenu::PostUpdate() {
	bool ret = true;
	return ret;
}

void MainMenu::OnEvent(j1UI_Element* element, FocusEvent event) {

	if ((element->type == UI_Type::BUTTON) && (event == FocusEvent::CLICKED))
	{
		GuiButton* button = (GuiButton*)element;
		switch (button->action)
		{
		case ButtonAction::PLAY:
			if (App->map->Load("Level1.tmx") == true)
			{
				int w, h;
				uchar* data = NULL;
				if (App->map->CreateWalkabilityMap(w, h, &data))
					App->pathfinding->SetMap(w, h, data);
				RELEASE_ARRAY(data);
			}

			App->render->camera.x = App->scene->initial_camera_position.x;
			App->render->camera.y = App->scene->initial_camera_position.y;

			App->gui->DestroyAllGui();
			if (App->console->isVisible) App->console->DestroyInterface();
			App->scene->CreateScreenUI();
			break;

		case ButtonAction::CONTINUE:
			App->gui->DestroyAllGui();
			App->LoadGame();
			App->scene->visible_menu = Menu::NO_MENU;
			App->scene->CreateScreenUI();
			break;

		case ButtonAction::SETTINGS:
			App->gui->DestroyAllGui();

			CreateSettingsScreen();
			break;

		case ButtonAction::CREDITS:
			if (visible_menu == Menu::MAIN_MENU) {
				App->gui->DestroyAllGui();
				CreateCreditsScreen();
				visible_menu = Menu::CREDITS;
			}
			else {
				ShellExecuteA(NULL, "open", "https://github.com/Avilgor/DesarrolloVJ", NULL, NULL, SW_SHOWNORMAL);
			}
			break;

		case ButtonAction::GO_BACK:
			App->gui->DestroyAllGui();
			CreateMainScreen();
			break;

		case ButtonAction::CONTEXTUAL_1:
			if (visible_menu == Menu::CREDITS) {
				ShellExecuteA(NULL, "open", "https://github.com/Avilgor", NULL, NULL, SW_SHOWNORMAL);
			}
			else if (visible_menu == Menu::SETTINGS) {
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
				window_width = App->win->width;
				window_width = App->win->height;
			}
			break;

		case ButtonAction::CONTEXTUAL_2:
			if (visible_menu == Menu::CREDITS) {
				ShellExecuteA(NULL, "open", "https://github.com/xatrilu", NULL, NULL, SW_SHOWNORMAL);
			}
			break;

		case ButtonAction::CONTEXTUAL_3:
			if (visible_menu == Menu::CREDITS) {
				ShellExecuteA(NULL, "open", "", NULL, NULL, SW_SHOWNORMAL);
			}
			break;

		case ButtonAction::QUIT:
			App->quit = true;
			break;

		default:
			break;
		}
	}
}

void MainMenu::CreateMainScreen() {
	SDL_Rect camera;
	camera = App->render->camera;
	_TTF_Font* font = App->font->Load("fonts/Lotterdam.ttf", 50);

	GuiImage* background = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this);
	background->Init({ 0,0 }, { 0,0,(int)App->win->width, (int)App->win->height });
	background->texture = App->tex->Load("sprites/UI/MenuBackground.jpg");

	GuiImage* title = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this);
	title->Init({ 100,100 }, { 0,0,800, 150 });
	title->texture = App->tex->Load("sprites/UI/Title.png");

	GuiButton* startBtn = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	startBtn->Init({ 400, 330 }, { 3,390,804,296 }, { 206,547,200,72 }, { 206,547,200,72 }, "Start", ButtonAction::PLAY);

	GuiButton* continueBtn = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	continueBtn->Init({ 400, 440 }, { 3,390,804,296 }, { 206,622,196,71 }, { 206,622,200,72 }, "Continue", ButtonAction::CONTINUE);

	GuiButton* settingsBtn = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	settingsBtn->Init({ 400, 540 }, { 3,390,804,296 }, { 206,547,200,72 }, { 206,547,200,72 }, "Settings", ButtonAction::SETTINGS);

	GuiButton* exitBtn = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	exitBtn->Init({ 50, 626 }, { 8,622,196,71 }, { 206,622,196,71 }, { 206,622,200,72 }, "Exit", ButtonAction::QUIT);

	GuiButton* creditsBtn = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	creditsBtn->Init({ 780, 626 }, { 6,547,200,72 }, { 206,547,200,72 }, { 206,547,200,72 }, "Credits", ButtonAction::CREDITS);

	visible_menu = Menu::MAIN_MENU;
}

void MainMenu::CreateSettingsScreen() 
{

	GuiImage* background = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this);
	background->Init({ 0,0 }, { 0,0,(int)App->win->width,(int)App->win->height });
	background->texture = App->tex->Load("sprites/UI/MenuBackground.jpg");

	GuiImage* menuBackground = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this);
	menuBackground->Init({ 250, 250 }, { 0,0,512,264 });

	GuiButton* backBtn = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	backBtn->Init({ 20, 20 }, { 897,618,114,94 }, { 897,618,114,94 }, { 897,618,114,94 }, "", ButtonAction::GO_BACK);

	GuiButton* fullscreenBtn = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	fullscreenBtn->Init({ 300,410 }, { 206, 697, 49,53 }, { 206, 697, 49,53 }, { 262,697,49,53 }, "", ButtonAction::CONTEXTUAL_1, true);

	GuiText* fullscreenTxt = (GuiText*)App->gui->CreateUIElement(UI_Type::TEXT, this, nullptr, false, true);
	fullscreenTxt->Init({ 380,406 }, "Fullscreen");

	visible_menu = Menu::SETTINGS;
}

void MainMenu::CreateCreditsScreen() {

	GuiImage* background = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this);
	background->Init({ 0,0 }, { 0,0,(int)App->win->width,(int)App->win->height });
	background->texture = App->tex->Load("sprites/UI/MenuBackground.jpg");

	GuiImage* panel = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this);
	panel->Init({ 140,160 }, { 0,1062,726,522 });

	GuiButton* go_back_button = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	go_back_button->Init({ 20, 20 }, { 897,618,114,94 }, { 897,618,114,94 }, { 897,618,114,94 }, "", ButtonAction::GO_BACK);

	GuiImage* title = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this);
	title->Init({ 100, 80 }, { 0,0,800, 150 });
	title->texture = App->tex->Load("sprites/UI/Title.png");

	GuiText* by = (GuiText*)App->gui->CreateUIElement(UI_Type::TEXT, this, nullptr);
	by->Init({ 470, 200 }, "by");

	/*GuiText* studio = (GuiText*)App->gui->CreateUIElement(UI_Type::TEXT, this, nullptr);
	studio->Init({ 380,250 }, " ");*/

	GuiButton* webpage = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	webpage->Init({ 420,520 }, { 1036,618,137,140 }, { 1038,619,138,140 }, { 1038,619,138,140 }, "", ButtonAction::CREDITS);

	GuiButton* adria = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	adria->Init({ 180,390 }, { 6,547,200,72 }, { 206,547,200,72 }, { 206,547,200,72 }, "Adri� Avila", ButtonAction::CONTEXTUAL_1);

	GuiButton* xavier = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	xavier->Init({ 625,390 }, { 6,547,200,72 }, { 206,547,200,72 }, { 206,547,200,72 }, "Xavier Trillo", ButtonAction::CONTEXTUAL_2);

	GuiButton* license = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	license->Init({ 780,25 }, { 8,622,196,71 }, { 206,622,196,71 }, { 206,622,200,72 }, "License", ButtonAction::CONTEXTUAL_3);

	visible_menu = Menu::CREDITS;
}