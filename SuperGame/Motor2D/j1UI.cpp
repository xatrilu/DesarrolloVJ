#include "j1App.h"
#include "p2Log.h"
#include "p2Defs.h"
#include "j1UI.h"
#include "j1Render.h"
#include "j1Scene.h"
#include "j1Player.h"
#include "j1Textures.h"
#include "brofiler/Brofiler/Brofiler.h"


j1UI::j1UI() : j1Module() {
	name.create("ui");
}

j1UI :: ~j1UI() {}

bool j1UI::Awake(pugi::xml_node& config) {
	LOG("Loading UI");
	transition_speed = config.child("transition_speed").attribute("value").as_float();
	return true;
}

bool j1UI::Start() {
	//transition values
	camera = &App->render->camera;
	pause_tex = App->tex->Load("sprites/UI/pause.png");

	left_square = {
		(int)(-camera->x - camera->w * 0.5f),
		(int)camera->y,
		(int)(camera->w * 0.5f),
		camera->h };

	right_square = { 
		camera->x + camera->w,
		camera->y,
		(int)(camera->w * 0.5f), 
		camera->h};
	return true;
}

bool j1UI::CleanUp() {
	LOG("Freeing UI");
	App->tex->UnLoad(pause_tex);
	pause_tex = nullptr;
	return true;
}

bool j1UI::Update(float dt) {
	BROFILER_CATEGORY("UIUpdate", Profiler::Color::BlueViolet)
	bool ret = true;
	if (transition) LevelTransition(dt);
	return ret;
}
bool j1UI::PostUpdate() {
	BROFILER_CATEGORY("UIPostUpdate", Profiler::Color::LawnGreen)

	if (transition) {
		App->render->DrawQuad(left_square, 0, 0, 0, 255);
		App->render->DrawQuad(right_square, 0, 0, 0, 255);
	}

	if (App->pause) App->render->Blit(pause_tex, -camera->x + 150, -camera->y + 294, NULL);

	return true;
}
void j1UI::LevelTransition(float dt) {
	SDL_Rect screen = { 0, 0, camera->w,camera->h };
	switch (direction)
	{
	case CLOSE:
		if (left_square.x <= -camera->x)
		{
			left_square.x += transition_speed * dt;
			right_square.x -= transition_speed * dt;
			App->render->DrawQuad(left_square, 0, 0, 0, 255);
			App->render->DrawQuad(right_square, 0, 0, 0, 255);
			App->scene->blocked_camera = true;
			App->entities->blocked_movement = true;
		}
		else direction = STATIC;
		break;
	case STATIC:
		time = SDL_GetTicks();
		if (time - transition_moment >= transition_time * 1000)
		{
			left_square.x = -camera->x;
			right_square.x = -camera->x + camera->w * 0.5f;

			if ((App->scene->current_level == LEVEL_1) && (App->scene->want_to_load == LEVEL_2)) 
			{
				App->scene->LevelChange(LEVEL_2, LEVEL_1);
			}
			else if ((App->scene->current_level == LEVEL_2) && (App->scene->want_to_load == LEVEL_1)) 
			{
				App->scene->LevelChange(LEVEL_1, LEVEL_2);
			}
			else
			{
				App->scene->ResetLevel();
			}
			direction = OPEN;
		}
		break;
	case OPEN:
		if (left_square.x + left_square.w > -camera->x)
		{
			left_square.x -= transition_speed * dt;
			right_square.x += transition_speed * dt;
			//LOG("Left square x: %i camera x: %i", left_square.x, camera->x);
		}
		else
		{
			transition = false;
			App->scene->blocked_camera = false;
			App->entities->blocked_movement = false;
			direction = CLOSE;
		}
		break;
	default:
		break;
	}
}

void j1UI::ResetTransition(TransitionState state) {

	left_square.y = -camera->y;
	right_square.y = -camera->y;

	if (state == CLOSE)
	{
		left_square.x = -camera->x - left_square.w;
		right_square.x = -camera->x + camera->w;
	}
	else
	{
		left_square.x = -camera->x;
		right_square.x = -camera->x + camera->w * 0.5f;
	}
}