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
	max_time = 200;
	time_star1 = 100;
	time_star2 = 150;
	time_star3 = 190;
	camera_margin = 5;
	initial_camera_position = { 0,0 };
	showing_menu = false;
	visible_menu = Menu::MAIN_MENU;
	current_level = Map::NO_MAP;
	on_screen_lives = 0;
	on_screen_score = 0;
	on_screen_stars = 0;
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
	/*
	if (App->map->Load("Level1.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	/*
	if (App->map->Load("Level2.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}
	*/
	App->console->CreateCommand("map_", this, "Change map");

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	BROFILER_CATEGORY("ScenePreUpdate", Profiler::Color::HotPink);
	// debug pathfing ------------------
	/*
	static iPoint origin;
	static bool origin_selected = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (origin_selected == true)
		{

			App->pathfinding->CreatePath(origin, p);
			origin_selected = false;
		}
		else
		{
			origin = p;
			origin_selected = true;
		}
	}
	*/

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
	if (visible_menu == Menu::SCREEN_UI)
	{
		if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
			App->fadeBlack->FadeToBlack(current_level, LEVEL_1);
		}

		if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
			App->fadeBlack->FadeToBlack(current_level, LEVEL_2);
		}

		if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
			App->fadeBlack->FadeToBlack(current_level, current_level);
			App->entities->player_pointer->position = App->entities->player_pointer->initialPosition;
		}

	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame();

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		App->LoadGame();

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		if (visible_menu != Menu::MAIN_MENU) {
			if (visible_menu == Menu::SCREEN_UI) {
				CreatePauseMenu();
				visible_menu = Menu::PAUSE;
				App->entities->blocked_movement = true;
			}
			else {
				DestroySpecificMenu(Menu::PAUSE);
				CreateScreenUI();
				visible_menu = Menu::SCREEN_UI;
				App->entities->blocked_movement = false;
			}
		}
	}

	//camera window ------------------
	if (!blocked_camera) {
		if (!camera_manual_control)
		{
			if (((player_position->x < camera_frame_x_center)) && (-camera->x > camera_margin)) {
				App->render->camera.x += floor(CAMERA_SPEED * dt);
				//camera_frame.x -= floor(CAMERA_SPEED * dt);
			}

			if ((player_position->x + App->entities->player_pointer->current_animation->GetCurrentFrame().w > camera_frame_x_center) && (-camera->x + camera->w < App->map->data.width * App->map->data.tile_width - 10)) {
				App->render->camera.x -= floor(CAMERA_SPEED * dt);
				//camera_frame.x += floor(CAMERA_SPEED * dt);
			}

			if (((player_position->y < camera_frame_y_center)) && (camera_frame.y - camera_frame_y_margin > camera_margin)) {
				App->render->camera.y += floor(CAMERA_SPEED * dt);
			}

			if (((player_position->y + App->entities->player_pointer->current_animation->GetCurrentFrame().h > camera_frame_y_center)) && (-camera->y + camera->h < App->map->data.height * App->map->data.tile_height - camera_margin)) {
				App->render->camera.y -= floor(CAMERA_SPEED * dt);
			}
		}
	}
	//camera manual control --------------
	if (!App->console->isVisible) {
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) {
			App->render->camera.y += floor(CAMERA_SPEED * dt);
			camera_manual_control = true;
		}

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) {
			App->render->camera.y -= floor(CAMERA_SPEED * dt);
			camera_manual_control = true;
		}

		if ((App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) && (App->render->camera.x < 0)) {
			App->render->camera.x += floor(CAMERA_SPEED * dt);
			camera_manual_control = true;
		}

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) {
			App->render->camera.x -= floor(CAMERA_SPEED * dt);
			camera_manual_control = true;
		}
	}

	App->map->Draw();

	UpdateScreenUI();

	// Debug pathfinding ------------------------------
	/*
	int x, y;
	SDL_Rect Debug_rect = { 0,0,32,32 };
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	//p = App->map->WorldToMap(p.x, p.y);
	//p = App->map->MapToWorld(p.x, p.y);

	//App->render->Blit(debug_tex, p.x, p.y);
	Debug_rect.x = p.x;
	Debug_rect.y = p.y;
	if (App->collision->debug) App->render->DrawQuad(Debug_rect, 0, 0, 255,80);

	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		Debug_rect.x = pos.x;
		Debug_rect.y = pos.y;
		if (App->collision->debug) App->render->DrawQuad(Debug_rect, 90, 850, 230, 80);
	}
	*/

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	BROFILER_CATEGORY("ScenePostUpdate", Profiler::Color::HotPink);
	bool ret = true;

	return ret;
}

bool j1Scene::CleanUp()
{
	LOG("Freeing scene");
	GameOver();
	return true;
}

bool j1Scene::Save(pugi::xml_node& data) const {

	//pugi::xml_node cam_frame = data.append_child("camera_frame");
	pugi::xml_node level = data.append_child("level");
	pugi::xml_node time = data.append_child("time_left");

	//cam_frame.append_attribute("x") = camera_frame.x;
	//cam_frame.append_attribute("y") = camera_frame.y;

	if (current_level == LEVEL_1) level.append_attribute("number") = 1;
	if (current_level == LEVEL_2) level.append_attribute("number") = 2;

	time.append_attribute("value") = time_left;

	return true;
}

bool j1Scene::Load(pugi::xml_node& data)
{
	//camera_frame.x = data.child("position").attribute("x").as_int();
	//camera_frame.y = data.child("position").attribute("y").as_int();

	if ((current_level == LEVEL_1) && (data.child("level").attribute("number").as_int() == 2)) LevelChange(LEVEL_2, LEVEL_1);
	if ((current_level == LEVEL_2) && (data.child("level").attribute("number").as_int() == 1)) LevelChange(LEVEL_1, LEVEL_2);

	if (current_level == NO_MAP) {
		if (data.child("level").attribute("number").as_int() == 1)
			LevelChange(NO_MAP, LEVEL_1);

		if (data.child("level").attribute("number").as_int() == 2)
			LevelChange(NO_MAP, LEVEL_2);
	}

	time_left = data.child("time_left").attribute("value").as_int();
	time_discount = max_time - time_left;
	timer.Start();

	return true;
}

void j1Scene::OnEvent(j1UI_Element* element, FocusEvent event) {
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
			visible_menu = Menu::SCREEN_UI;
			break;

		case ButtonAction::SETTINGS:
			DestroySpecificMenu(Menu::PAUSE);
			pause_menu.clear();
			CreateSettingsScreen();
			visible_menu = Menu::SETTINGS;

			break;
		case ButtonAction::CONTEXTUAL_1:
			if (visible_menu == Menu::SETTINGS) {
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
				//window_height = App->win->width;
				//window_height = App->win->height;
			}

		case ButtonAction::GO_BACK:
			DestroySpecificMenu(Menu::SETTINGS);
			settings_screen.clear();
			CreatePauseMenu();
			visible_menu = Menu::PAUSE;
			break;

			//go to main menu
		case ButtonAction::QUIT:
			GameOver();
			break;

		default:
			break;
		}
	}
}

void j1Scene::UpdateScreenUI() {

	if ((lives.start != nullptr) && (on_screen_lives > App->entities->player_pointer->lives))
	{
		App->gui->DestroyUIElement(lives.end->data);
		lives.del(lives.end);
		on_screen_lives--;
	}

	if ((stars.start != nullptr) && (timer.Read() > time_star1 * 1000)) {

		App->gui->DestroyUIElement(stars.end->data);
		stars.del(stars.At(3));
	}

	if ((stars.start != nullptr) && (timer.Read() > time_star2 * 1000)) {

		App->gui->DestroyUIElement(stars.end->data);
		stars.del(stars.At(2));
	}

	if ((stars.start != nullptr) && (timer.Read() > time_star3 * 1000)) {

		App->gui->DestroyUIElement(stars.end->data);
		stars.del(stars.At(1));
	}

	if (time_text != nullptr)
	{
		if (performance_timer.Read() > 1000)
		{
			time_left = relative_max_time - timer.ReadSec();
			p2SString temp("%i", time_left);
			time_count->text.Clear();
			if (temp.Length() > 0) time_count->text = temp;
			time_count->UpdateText();
			performance_timer.Start();
		}

		if (on_screen_score != App->entities->player_pointer->score) {
			p2SString coin("     %i points", App->entities->player_pointer->score);
			score->text.Clear();
			score->text = coin;
			score->UpdateText();
			on_screen_score = App->entities->player_pointer->score;
		}
	}
}

void j1Scene::OnCommand(p2SString command) {
	char* given_command = (char*)command.GetString();
	char map1[11] = "map_level1";
	char map2[11] = "map_level2";

	if (strcmp(given_command, map1) == 0)
	{
		App->gui->DestroyAllGui();
		App->console->DestroyInterface();
		LOG("Loading level 1");
		LevelChange(current_level, LEVEL_1);
		CreateScreenUI();
	}
	if (strcmp(given_command, map2) == 0)
	{
		App->gui->DestroyAllGui();
		App->console->DestroyInterface();
		LOG("Loading level 2");
		LevelChange(current_level, LEVEL_2);
		CreateScreenUI();
	}

}

void j1Scene::GameOver()
{
	App->gui->DestroyAllGui();
	on_screen_ui.clear();
	settings_screen.clear();
	pause_menu.clear();
	App->map->CleanUp();
	App->entities->DestroyAllEntities();
	on_screen_lives = 0;
	on_screen_score = 0;
	App->entities->player_pointer->score = 0;
	relative_max_time = max_time;
	App->render->camera.x = 0;
	App->render->camera.y = 0;
	score = nullptr;
	time_text = nullptr;
	time_count = nullptr;
	visible_menu = Menu::MAIN_MENU;
	current_level = Map::NO_MAP;
	App->mainMenu->CreateMainScreen();
}

void j1Scene::CreatePauseMenu() {

	GuiImage* menu_background = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this);
	menu_background->Init({ 250, 250 }, { 0,0,512,264 });
	pause_menu.add(menu_background);

	GuiImage* pause_text = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this, menu_background);
	pause_text->Init({ 298, 155 }, { 897,0,420,104 });
	pause_menu.add(pause_text);

	GuiButton* resume_button = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, menu_background, false, true);
	resume_button->Init({ 445, 325 }, { 658,837,117,120 }, { 658,837,117,120 }, { 775,837,117,120 }, "", ButtonAction::CONTINUE);
	pause_menu.add(resume_button);

	GuiButton* home_button = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, menu_background, false, true);
	home_button->Init({ 285, 325 }, { 416,837,117,120 }, { 416,837,117,120 }, { 534,837,117,120 }, "", ButtonAction::QUIT);
	pause_menu.add(home_button);

	GuiButton* settings_button = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, menu_background, false, true);
	settings_button->Init({ 605,325 }, { 658,596,117,120 }, { 658,596,117,120 }, { 777,596,117,120 }, "", ButtonAction::SETTINGS);
	pause_menu.add(settings_button);

	if ((stars.start != nullptr) && (timer.Read() < time_star1 * 1000)) {

		star1 = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this, nullptr);
		star1->Init({ 420 , 225 }, { 589,124, 76, 69 });

		star2 = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this, nullptr);
		star2->Init({ 470 , 225 }, { 589,124, 76, 69 });

		star3 = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this, nullptr);
		star3->Init({ 520 , 225 }, { 589,124, 76, 69 });

		pause_menu.add(star1);
		pause_menu.add(star2);
		pause_menu.add(star3);
	}

	if ((stars.start != nullptr) && (timer.Read() < time_star2 * 1000) && (timer.Read() > time_star1 * 1000)) {
		star1 = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this, nullptr);
		star1->Init({ 420 , 225 }, { 589,124, 76, 69 });

		star2 = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this, nullptr);
		star2->Init({ 470 , 225 }, { 589,124, 76, 69 });

		pause_menu.add(star1);
		pause_menu.add(star2);
	}

	if ((stars.start != nullptr) && (timer.Read() < time_star3 * 1000) && (timer.Read() > time_star2 * 1000)) {
		star3 = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this, nullptr);
		star3->Init({ 420 , 225 }, { 589,124, 76, 69 });

		pause_menu.add(star1);
	}


}

void j1Scene::CreateSettingsScreen() {

	GuiImage* menu_background = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this);
	menu_background->Init({ 250,250 }, { 0,0,512,264 });
	settings_screen.add(menu_background);

	GuiButton* go_back_button = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, menu_background, false, true);
	go_back_button->Init({ 200, 200 }, { 895,474,96,99 }, { 895,474,96,99 }, { 993,474,96,99 }, "", ButtonAction::GO_BACK);
	settings_screen.add(go_back_button);

	GuiButton* fullscreen_button = (GuiButton*)App->gui->CreateUIElement(UI_Type::BUTTON, this, nullptr, false, true);
	fullscreen_button->Init({ 300,410 }, { 206, 697, 49,53 }, { 206, 697, 49,53 }, { 262,697,49,53 }, "", ButtonAction::CONTEXTUAL_1, true);
	settings_screen.add(fullscreen_button);

	GuiText* fullscreen_text = (GuiText*)App->gui->CreateUIElement(UI_Type::TEXT, this, nullptr, false, true);
	fullscreen_text->Init({ 380,406 }, "Fullscreen");
	settings_screen.add(fullscreen_text);

	/*GuiSlider* volume_slider = (GuiSlider*)App->gui->CreateUIElement(UI_Type::SLIDER, this);
	volume_slider->Init({ 285, 350 }, { 7,281,438,37 }, { 56,750,69,52 });
	settings_screen.add(volume_slider);*/

	GuiText* music_volume = (GuiText*)App->gui->CreateUIElement(UI_Type::TEXT, this, nullptr, false, true);
	music_volume->Init({ 290,280 }, "Music Volume");
	settings_screen.add(music_volume);
}

void j1Scene::CreateScreenUI()
{
	int livesXpos = 50;
	int livesXDistance = 90;

	int starsXpos = 400;
	int starsXDistance = 50;

	int nostarsXpos = 400;
	int nostarsXDistance = 50;

	int timeXpos = 700;

	for (int i = 0; i < App->entities->player_pointer->lives; i++)
	{

		GuiImage* life = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this, nullptr);
		life->Init({ livesXpos , 20 }, { 667,15,68,63 });
		livesXpos += livesXDistance;
		lives.add(life);
		on_screen_lives++;
		on_screen_ui.add(life);
	}

	for (int i = 0; i < 3; i++)
	{
		GuiImage* star = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this, nullptr);
		star->Init({ starsXpos , 20 }, { 589,124, 76, 69 });
		starsXpos += starsXDistance;
		stars.add(star);
		on_screen_ui.add(star);
	}

	coins = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this, nullptr);
	coins->Init({ 30, 625 }, { 9,865,294,69 });
	on_screen_ui.add(coins);

	timer_background = (GuiImage*)App->gui->CreateUIElement(UI_Type::IMAGE, this, nullptr);
	timer_background->Init({ 700, 10 }, { 240,978, 289,78 });
	on_screen_ui.add(timer_background);

	time_left = time_left - timer.ReadSec() * 0.001f;
	time_text = (GuiText*)App->gui->CreateUIElement(UI_Type::TEXT, this, timer_background);
	time_text->Init({ 730, 20 }, "Time: ");
	on_screen_ui.add(time_text);

	time_count = (GuiText*)App->gui->CreateUIElement(UI_Type::TEXT, this, timer_background);
	p2SString temp("%i", time_left);
	time_count->Init({ 810, 20 }, temp);
	on_screen_ui.add(time_count);

	score = (GuiText*)App->gui->CreateUIElement(UI_Type::TEXT, this, coins);
	p2SString coin("     %i points", App->entities->player_pointer->score);
	score->Init({ 100, 632 }, coin);
	on_screen_ui.add(score);

	visible_menu = Menu::SCREEN_UI;
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

			current_level = LEVEL_1;
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

			current_level = LEVEL_2;
		}
	}
	else
	{
		App->entities->RellocateEntities();
	}
}

void j1Scene::DestroySpecificMenu(Menu menu) {
	p2List_item<j1UI_Element*>* item = nullptr;

	if (menu == Menu::SCREEN_UI)
		item = on_screen_ui.start;
	if (menu == Menu::PAUSE)
		item = pause_menu.start;
	if (menu == Menu::SETTINGS)
		item = settings_screen.start;

	for (item; item != nullptr; item = item->next)
	{
		App->gui->DestroyUIElement(item->data);
	}
}