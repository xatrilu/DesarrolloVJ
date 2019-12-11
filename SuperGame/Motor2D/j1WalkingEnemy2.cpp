#include "j1WalkingEnemy2.h"
#include "j1Player.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Collision.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Audio.h"
#include "j1UI.h"
#include "j1Particles.h"
#include "j1EntityManager.h"
#include "brofiler/Brofiler/Brofiler.h"

j1WalkingEnemy2::j1WalkingEnemy2() :j1Entity(EntityType::WALKING_ENEMY2) {
	name.create("walking_enemy2");

	//variable declaration from EntityManager
	player = App->entities->player;
	gravity = App->entities->gravity;
	max_falling_speed = App->entities->max_falling_speed;
	type = EntityType::WALKING_ENEMY2;

	//copy variables from reference_enemy
	if (App->entities->reference_walking_enemy2 != nullptr)
	{
		speed = App->entities->reference_walking_enemy2->speed;
		health = App->entities->reference_walking_enemy2->health;
		damage = App->entities->reference_walking_enemy2->damage;
		detection_range = App->entities->reference_walking_enemy2->detection_range;
		texture = App->entities->reference_walking_enemy2->texture;

		animations = App->entities->reference_walking_enemy2->animations;
		idle = *animations.At(0)->data;
		attack = *animations.At(1)->data;
		run = *animations.At(2)->data;
		die_fx = App->entities->reference_walking_enemy2->die_fx;

		current_animation = &idle;
	}

	//lastPosition = position;
	//last_animation = &idle;
	lastPosition = position;
	flip = SDL_FLIP_HORIZONTAL;

	//colliders
	collider = App->collision->AddCollider({ 27,47,60,70 }, COLLIDER_ENEMY, (j1Module*)this);
	raycast = App->collision->AddCollider({ 16,34,8,10 }, COLLIDER_ENEMY, (j1Module*)this);
	//SDL_Rect attack_collider_rect{0,0,26,12};

}

j1WalkingEnemy2::~j1WalkingEnemy2() {
	/*
	App->tex->UnLoad(texture);
	texture = nullptr;
	collider->to_delete = true;
	collider = nullptr;
	raycast->to_delete = true;
	raycast = nullptr;
	*/
}

bool j1WalkingEnemy2::Awake(pugi::xml_node& config) {
	bool ret = true;

	speed.x = speed.y = config.child("running_speed").attribute("value").as_int();
	health = config.child("health").attribute("value").as_int();
	damage = config.child("damage").attribute("value").as_int();
	detection_range = config.child("detection_range").attribute("value").as_int();

	LoadAnimations("Animations_Enemy2.tmx");

	return ret;
}

bool j1WalkingEnemy2::Update(float dt) {
	BROFILER_CATEGORY("WalkingEnemy2Update", Profiler::Color::Orange)
		bool ret = true;

	lastPosition = position;
	last_animation = current_animation;
	//what to do when getting to a gap
	if (last_collider != nullptr)
	{
		if (!raycast->CheckCollision(last_collider->rect))
		{
			grounded = false;
			//current_speed.x = -current_speed.x;
		}
	}

	//guard path
	//if ((position.x < path_minimum)||(position.x > path_maximum)) current_speed.x -= current_speed.x;

	//pathfind
	PathfindtoPlayer(detection_range, player);

	//movement
	if ((path_to_player != nullptr) && (path_to_player->Count() != 0))
	{
		//compare position to tile to go
		int i = 0;
		iPoint current_map_position = App->map->WorldToMap(position.x, position.y);
		iPoint tile_to_go;
		iPoint destination;
		tile_to_go.x = path_to_player->At(i)->x;
		tile_to_go.y = path_to_player->At(i)->y;
		destination.x = path_to_player->At(path_to_player->Count() - 1)->x;
		destination.y = path_to_player->At(path_to_player->Count() - 1)->y;

		if (tile_to_go.y < current_map_position.y) i++;

		if (current_map_position.x == tile_to_go.x)
		{
			i++;
			if (i > 2) tile_to_go = App->map->WorldToMap(path_to_player->At(i)->x, path_to_player->At(i)->y);
		}

		if (current_map_position.x > tile_to_go.x) {
			//LOG("Going left");
			if ((current_map_position.DistanceManhattan(destination) < attacking_range + 1) && ((position.x - player->position.x) < 40))
				state = ATTACK;
			else state = RUN_BACKWARD;
		}
		if (current_map_position.x < tile_to_go.x) {
			//LOG("Going right");
			if (current_map_position.DistanceManhattan(destination) <= attacking_range)
				state = ATTACK;
			else state = RUN_FORWARD;
		}
		if (current_map_position.y > tile_to_go.y) {
			//LOG("Going up");
			//position.y -= 3;
		}
		if (current_map_position.y < tile_to_go.y) {
			//LOG("Going down");
		}
	}

	//state machine
	switch (state)
	{
	case IDLE:
		current_animation = &idle;
		if (collider != nullptr)
			if (flip == SDL_FLIP_NONE)
				collider->SetPos(position.x + 16, position.y + 30);
			else
				collider->SetPos(position.x + 22, position.y + 30);
		break;
	case RUN_FORWARD:
		current_animation = &run;
		current_speed.x = speed.x;
		flip = SDL_FLIP_NONE;
		if (collider != nullptr) {
			collider->SetPos(position.x + 16, position.y + 30);
			raycast->SetPos(position.x + 40, position.y + current_animation->GetCurrentFrame().h);
		}
		break;
	case RUN_BACKWARD:
		current_animation = &run;
		current_speed.x = -speed.x;
		flip = SDL_FLIP_HORIZONTAL;
		if (collider != nullptr) {
			collider->SetPos(position.x + 16, position.y + 30);
			raycast->SetPos(position.x + 16, position.y + current_animation->GetCurrentFrame().h);
		}
		break;
	case FALL:
		current_animation = &idle;
		if (collider != nullptr)
			if (flip == SDL_FLIP_NONE)
				collider->SetPos(position.x + 16, position.y + 30);
			else
				collider->SetPos(position.x + 22, position.y + 30);
		break;
	case ATTACK:
		App->audio->PlayFx(attack_fx);
		current_animation = &attack;
		current_speed.x = 0;
		if (attack_collider == nullptr) {
			attack_collider = App->collision->AddCollider({ 0,0,26,12 }, COLLIDER_ENEMY);
		}
		if (collider != nullptr) {
			if (flip == SDL_FLIP_NONE) {
				collider->SetPos(position.x + 56, position.y + 80);
				attack_collider->SetPos(position.x + 80, position.y + 92);
			}
			else {
				collider->SetPos(position.x + 82, position.y + 80);
				attack_collider->SetPos(position.x, position.y + 84);
			}
		}
		break;
	case DIE:
		//current_animation = &die;
		break;
	case JUMP:
		//current_animation = &jump;
		break;
	default:
		break;
	}

	if (last_collider != nullptr)
	{
		if (!raycast->CheckCollision(last_collider->rect))
		{
			grounded = false;
			current_speed.x = -current_speed.x;
			if (lastPosition.x < position.x) {
				position.x = lastPosition.x;
			}
		}
	}

	//Movement Control

	if (!grounded) {
		if (current_speed.y > max_falling_speed) {
			current_speed.y -= gravity * dt;
		}
		position.y -= current_speed.y * dt;
	}
	if (grounded)
	{
		position.x += current_speed.x * dt;
	}

	if ((attack_collider != nullptr) && (state != ATTACK)) {
		attack_collider->to_delete = true;
		attack_collider = nullptr;
	}

	return ret;
}

bool j1WalkingEnemy2::PostUpdate() {
	BROFILER_CATEGORY("WalkingEnemy2PostUpdate", Profiler::Color::Orange)
		bool ret = true;
	if (current_animation == nullptr) current_animation = last_animation;

	App->render->Blit(texture, position.x, position.y, &current_animation->GetCurrentFrame(), flip);

	return ret;
}


void j1WalkingEnemy2::OnCollision(Collider* c1, Collider* c2) {

	if (c1 == raycast)
	{
		last_collider = c2;
	}

	switch (c2->type)
	{
	case COLLIDER_WALL:
		if (position.y + current_animation->GetCurrentFrame().h < c2->rect.y + COLLIDER_MARGIN)
		{
			grounded = true;
			position.y = c2->rect.y - current_animation->GetCurrentFrame().h;
			current_speed.y = 0;
			state = IDLE;
		}

		if (position.y + current_animation->GetCurrentFrame().h > c2->rect.y) {
			position.x = lastPosition.x;
		}
		if (position.y > c2->rect.y + c2->rect.h - COLLIDER_MARGIN)
		{
			position.y = c2->rect.y + c2->rect.h;
			current_speed.y = 0;
		}
		if ((position.y > c2->rect.y) && (position.x > c2->rect.x) && (position.x + current_animation->GetCurrentFrame().w < c2->rect.x + c2->rect.w) && (position.x < c2->rect.x + c2->rect.w))
		{
			position.y = lastPosition.y;
			if (lastPosition.y + current_animation->GetCurrentFrame().h > c2->rect.y) {
				//position.y = c2->rect.y - current_animation->GetCurrentFrame().h;
			}
		}
		break;
	case COLLIDER_PLATFORM:
		if (position.y + current_animation->GetCurrentFrame().h < c2->rect.y + COLLIDER_MARGIN)
		{
			grounded = true;
			position.y = c2->rect.y - current_animation->GetCurrentFrame().h;
			current_speed.y = 0;
			state = IDLE;
		}

		if (position.y + current_animation->GetCurrentFrame().h > c2->rect.y) {
			position.x = lastPosition.x;
		}
		if (position.y > c2->rect.y + c2->rect.h - COLLIDER_MARGIN)
		{
			position.y = c2->rect.y + c2->rect.h;
			current_speed.y = 0;
		}
		if ((position.y > c2->rect.y) && (position.x > c2->rect.x) && (position.x + current_animation->GetCurrentFrame().w < c2->rect.x + c2->rect.w) && (position.x < c2->rect.x + c2->rect.w))
		{
			position.y = lastPosition.y;
			if (lastPosition.y + current_animation->GetCurrentFrame().h > c2->rect.y) {
				//position.y = c2->rect.y - current_animation->GetCurrentFrame().h;
			}
		}
		break;
	case COLLIDER_PLAYER:
		if (!particles_created)
		{
			App->particles->AddParticle(App->particles->dust, collider->rect.x, collider->rect.y);
			particles_created = true;
		}
		App->entities->DestroyEntity(this);
		if (attack_collider != nullptr)
		{
			attack_collider->to_delete = true;
			attack_collider = nullptr;
		}

		break;
	default:
		break;
	}
}