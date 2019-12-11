#include <math.h>
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1FadeToBlack.h"
#include "j1Render.h"
#include "SDL/include/SDL_render.h"
#include "j1Window.h"
#include "SDL/include/SDL_timer.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1EntityManager.h"
#include "j1Player.h"
#include "j1Particles.h"


j1FadeToBlack::j1FadeToBlack(){}

j1FadeToBlack::~j1FadeToBlack() {}

// Load assets
bool j1FadeToBlack::Start()
{
	LOG("Preparing Fade Screen");
	screen = { 0, 0,(int)(App->win->width * App->win->scale),(int)(App->win->height * App->win->scale) };
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

// Update: draw background
bool j1FadeToBlack::PostUpdate()
{
	if (current_step == fade_step::none)
		return true;

	Uint32 now = SDL_GetTicks() - start_time;
	float normalized = MIN(1.0f, (float)now / (float)total_time);

	switch (current_step)
	{
		case fade_step::fade_to_black:
		{
			if (now >= total_time)
			{
				App->scene->LevelChange(map_off, map_on);

				App->entities->blocked_movement = false;

				total_time += total_time;
				start_time = SDL_GetTicks();
				current_step = fade_step::fade_from_black;
				App->entities->player->particles_created = false;
			}
		} break;

		case fade_step::fade_from_black:
		{
			normalized = 1.0f - normalized;
			if (App->entities->player->particles_created == false) {
				App->particles->AddParticle(App->particles->dust, App->entities->player->position.x - 10, App->entities->player->position.y, COLLIDER_NONE, 0, App->entities->player->flip);
				App->particles->AddParticle(App->particles->dust, App->entities->player->position.x, App->entities->player->position.y + 20, COLLIDER_NONE, 0, App->entities->player->flip);
				App->particles->AddParticle(App->particles->dust, App->entities->player->position.x - 10, App->entities->player->position.y + App->entities->player->current_animation->GetCurrentFrame().h - 22, COLLIDER_NONE, 0, App->entities->player->flip);
				App->particles->AddParticle(App->particles->dust, App->entities->player->position.x + 2, App->entities->player->position.y + App->entities->player->current_animation->GetCurrentFrame().h - 2, COLLIDER_NONE, 0, App->entities->player->flip);
				App->entities->player->particles_created = true;
				App->entities->player->isVisible = true;
			}
			if (now >= total_time) {
				current_step = fade_step::none;
				//App->entities->player->grounded = false;
				App->entities->player->particles_created = false;
				App->entities->player->isVisible = true;
			}
		} break;
	}

	// Finally render the black square with alpha on the screen
	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);

	return true;
}

bool j1FadeToBlack::FadeToBlack(Map map_off, Map map_on, float time)
{
	bool ret = false;

	if (current_step == fade_step::none)
	{
		this->map_off = map_off;
		this->map_on = map_on;
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;
	}

	return ret;
}
