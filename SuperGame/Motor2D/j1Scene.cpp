#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Player.h"
#include "j1Pathfinding.h"
#include "j1Collision.h"
#include "brofiler/Brofiler/Brofiler.h"
#include "j1EntityManager.h"
#include "j1FadeToBlack.h"
#include "j1Gui.h"
#include "MainMenu.h"
#include "ConsoleCommands.h"
#include "Console.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");

	blocked_camera = false;
	score = 0;
	camera_margin = 5;
	startCameraPos = { 0,0 };
	showing_menu = false;
	openMenu = Menu::MAIN_MENU;
	currentLevel = Map::NO_MAP;
	uiLives = 0;
	uiScore = 0;
	fullscreen = false;
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	folder.create(config.child("folder").child_value());
	camera_frame_x_margin = config.child("camera_frame").attribute("x").as_int();
	camera_frame_y_margin = config.child("camera_frame").attribute("y").as_int();
	camera_frame.w = config.child("camera_frame").attribute("w").as_int();
	camera_frame.h = config.child("camera_frame").attribute("h").as_int();
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{	
	App->console->CreateCommand("map_", this, "Change map");

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	BROFILER_CATEGORY("ScenePreUpdate", Profiler::Color::HotPink);
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	BROFILER_CATEGORY("SceneUpdate", Profiler::Color::HotPink)

	SDL_Rect* camera = &App->render->camera;
	iPoint* player_position = &App->entities->player_pointer->position;
	float		camera_frame_x_center = ceil(camera_frame.x + camera_frame.w * 0.5f);
	float		camera_frame_y_center = ceil(camera_frame.y + camera_frame.h * 0.5f);
	bool		camera_manual_control = false;
	camera_frame.x = -camera->x + camera_frame_x_margin;
	camera_frame.y = -camera->y + camera_frame_y_margin;

	//player inputs ---------------
	if (openMenu == Menu::SCREEN_UI)
	{
		if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) App->fadeBlack->FadeToBlack(currentLevel, LEVEL_1);

		if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) App->fadeBlack->FadeToBlack(currentLevel, LEVEL_2);

		if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) 
		{
			App->fadeBlack->FadeToBlack(currentLevel, currentLevel);
			App->entities->player_pointer->position = App->entities->player_pointer->initialPosition;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) App->SaveGame();

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) App->LoadGame();

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) 
	{
		if (openMenu != Menu::MAIN_MENU) 
		{
			if (openMenu == Menu::SCREEN_UI) 
			{
				CreatePauseMenu();
				openMenu = Menu::PAUSE;
				App->entities->blocked_movement = true;
			}
			else 
			{
				DestroySpecificMenu(Menu::PAUSE);
				CreateScreenUI();
				openMenu = Menu::SCREEN_UI;
				App->entities->blocked_movement = false;
			}
		}
	}

	//camera window ------------------
	if (!blocked_camera) {
		if (!camera_manual_control)
		{
			if (((player_position->x < camera_frame_x_center)) && (-camera->x > camera_margin)) 
				App->render->camera.x += floor(CAMERA_SPEED * dt);

			if ((player_position->x + App->entities->player_pointer->current_animation->GetCurrentFrame().w > camera_frame_x_center) && (-camera->x + camera->w < App->map->data.width * App->map->data.tile_width - 10)) 
				App->render->camera.x -= floor(CAMERA_SPEED * dt);

			if (((player_position->y < camera_frame_y_center)) && (camera_frame.y - camera_frame_y_margin > camera_margin)) 
				App->render->camera.y += floor(CAMERA_SPEED * dt);

			if (((player_position->y + App->entities->player_pointer->current_animation->GetCurrentFrame().h > camera_frame_y_center)) && (-camera->y + camera->h < App->map->data.height * App->map->data.tile_height - camera_margin)) 
				App->render->camera.y -= floor(CAMERA_SPEED * dt);
		}
	}

	//camera manual control --------------
	if (!App->console->visible) {
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) 
		{
			App->render->camera.y += floor(CAMERA_SPEED * dt);
			camera_manual_control = true;
		}

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) 
		{
			App->render->camera.y -= floor(CAMERA_SPEED * dt);
			camera_manual_control = true;
		}

		if ((App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) && (App->render->camera.x < 0)) 
		{
			App->render->camera.x += floor(CAMERA_SPEED * dt);
			camera_manual_control = true;
		}

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) 
		{
			App->render->camera.x -= floor(CAMERA_SPEED * dt);
			camera_manual_control = true;
		}
	}
	App->map->Draw();
	if (ScreenUI.count() != 0)
	{
		UpdateScreenUI();
	}
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	BROFILER_CATEGORY("ScenePostUpdate", Profiler::Color::HotPink);
	bool ret = true;

	return ret;
}

void j1Scene::CreatePauseMenu()
{
	GuiImage* menu_background = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this);
	menu_background->Init({ 200, 220 }, { 17,581,605,394 });
	pause_menu.add(menu_background);

	GuiImage* menuBackground = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this);
	menuBackground->Init({ 245, 280 }, { 192,327,517,76 });

	GuiText* pauseTxt = (GuiText*)App->gui->CreateUIElement(UI_Type::TEXT, this, nullptr, false, true);
	pauseTxt->Init({ 460,300 }, "PAUSE");

	GuiButton* resume_button = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, menu_background, false, true);
	resume_button->Init({ 550, 420 }, { 133, 470, 110,101 }, { 475, 477, 110,101 }, { 475,477,110,101 }, "RESUME", ButtonAction::CONTINUE);
	pause_menu.add(resume_button);

	GuiButton* home_button = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, menu_background, false, true);
	home_button->Init({ 330, 420 }, { 3,470,66,80 }, { 3,470,66,80 }, { 3,470,66,80 }, "MENU", ButtonAction::QUIT);
	pause_menu.add(home_button);
}

void j1Scene::CreateScreenUI()
{
	int livesXpos = 50;
	int livesXDistance = 90;

	for (int i = 0; i < App->entities->player_pointer->lives; i++)
	{
		GuiImage* life = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this, nullptr);
		life->Init({ livesXpos , 20 }, { 231,110,82,78 });
		livesXpos += livesXDistance;
		lives.add(life);
		uiLives++;
		ScreenUI.add(life);
	}

	stars = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this, nullptr);
	stars->Init({ 30, 625 }, { 236,16,73,79 });
	ScreenUI.add(stars);

	score = (GuiText*)App->gui->CreateUIElement(UI_Type::TEXT, this, stars);
	p2SString coin("%i points", App->entities->player_pointer->score);
	score->Init({ 130, 650 }, coin.GetString());
	ScreenUI.add(score);

	openMenu = Menu::SCREEN_UI;
}

bool j1Scene::Save(pugi::xml_node& data) const 
{

	pugi::xml_node level = data.append_child("level");
	pugi::xml_node time = data.append_child("time_left");

	if (currentLevel == LEVEL_1) level.append_attribute("number") = 1;
	if (currentLevel == LEVEL_2) level.append_attribute("number") = 2;

	return true;
}

bool j1Scene::Load(pugi::xml_node& data)
{
	if ((currentLevel == LEVEL_1) && (data.child("level").attribute("number").as_int() == 2)) LevelChange(LEVEL_2, LEVEL_1);
	if ((currentLevel == LEVEL_2) && (data.child("level").attribute("number").as_int() == 1)) LevelChange(LEVEL_1, LEVEL_2);

	if (currentLevel == NO_MAP) {
		if (data.child("level").attribute("number").as_int() == 1)
			LevelChange(NO_MAP, LEVEL_1);

		if (data.child("level").attribute("number").as_int() == 2)
			LevelChange(NO_MAP, LEVEL_2);
	}
	return true;
}

void j1Scene::OnEvent(j1UI_Element* element, FocusEvent event)
{
	if ((element->type == UI_Type::BUTTON) && (event == FocusEvent::CLICKED))
	{
		GuiButton* button = (GuiButton*)element;
		switch (button->action)
		{
		case ButtonAction::CONTINUE:
			DestroySpecificMenu(Menu::PAUSE);
			pause_menu.clear();
			App->entities->blocked_movement = false;
			showing_menu = false;
			openMenu = Menu::SCREEN_UI;
			break;

		case ButtonAction::SETTINGS:
			DestroySpecificMenu(Menu::PAUSE);
			pause_menu.clear();
			openMenu = Menu::SETTINGS;

			break;
		case ButtonAction::FULLSCREEN:
			if (openMenu == Menu::SETTINGS) {
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
			}

		case ButtonAction::BACK:
			DestroySpecificMenu(Menu::SETTINGS);
			CreatePauseMenu();
			openMenu = Menu::PAUSE;
			break;
		case ButtonAction::QUIT:
			GameOver();
			break;

		default:
			break;
		}
	}
}

void j1Scene::UpdateScreenUI() 
{
	if ((lives.start != nullptr) && (uiLives > App->entities->player_pointer->lives))
	{
		App->gui->DestroyUIElement(lives.end->data);
		lives.del(lives.end);
		uiLives--;
	}

	if (uiScore != App->entities->player_pointer->score) {
		p2SString coin("%i points", App->entities->player_pointer->score);
		score->text.Clear();
		score->text = coin.GetString();
		score->UpdateText();
		uiScore = App->entities->player_pointer->score;
	}
}

void j1Scene::Commands(p2SString command) 
{
	char* given_command = (char*)command.GetString();
	char map1[11] = "map_level1";
	char map2[11] = "map_level2";

	if (strcmp(given_command, map1) == 0)
	{
		App->gui->DestroyAllGui();
		App->console->DestroyInterface();
		LOG("Loading level 1");
		LevelChange(currentLevel, LEVEL_1);
		CreateScreenUI();
	}
	if (strcmp(given_command, map2) == 0)
	{
		App->gui->DestroyAllGui();
		App->console->DestroyInterface();
		LOG("Loading level 2");
		LevelChange(currentLevel, LEVEL_2);
		CreateScreenUI();
	}
}

void j1Scene::LevelChange(Map unloading_map, Map loading_map) {

	if (loading_map != unloading_map)
	{
		if (unloading_map != Map::NO_MAP)
			App->map->CleanUp();

		if (loading_map == LEVEL_1) {
			if (App->map->Load("Level1.tmx") == true)
			{
				int w, h;
				uchar* data = NULL;
				if (App->map->CreateWalkabilityMap(w, h, &data))
					App->pathfinding->SetMap(w, h, data);
				RELEASE_ARRAY(data);
			}
			currentLevel = LEVEL_1;
		}

		if (loading_map == LEVEL_2) {
			if (App->map->Load("Level2.tmx") == true)
			{
				int w, h;
				uchar* data = NULL;
				if (App->map->CreateWalkabilityMap(w, h, &data))
					App->pathfinding->SetMap(w, h, data);
				RELEASE_ARRAY(data);
			}
			currentLevel = LEVEL_2;
		}
	}
	else
	{
		App->entities->RellocateEntities();
	}
}

void j1Scene::GameOver()
{
	App->gui->DestroyAllGui();
	ScreenUI.clear();
	pause_menu.clear();
	App->map->CleanUp();
	App->entities->DestroyAllEntities();
	uiLives = 0;
	uiScore = 0;
	App->entities->player_pointer->score = 0;
	App->render->camera.x = 0;
	App->render->camera.y = 0;
	score = nullptr;
	openMenu = Menu::MAIN_MENU;
	currentLevel = Map::NO_MAP;
	App->mainMenu->CreateMainScreen();
}


void j1Scene::DestroySpecificMenu(Menu menu) {
	p2List_item<j1UI_Element*>* item = nullptr;

	if (menu == Menu::SCREEN_UI)
		item = ScreenUI.start;
	if (menu == Menu::PAUSE)
		item = pause_menu.start;

	for (item; item != nullptr; item = item->next)
	{
		App->gui->DestroyUIElement(item->data);
	}
}

bool j1Scene::CleanUp()
{
	LOG("Freeing scene");
	GameOver();
	return true;
}