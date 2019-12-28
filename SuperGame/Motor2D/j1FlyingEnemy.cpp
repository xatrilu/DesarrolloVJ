#include "j1FlyingEnemy.h"
#include "j1Entity.h"
#include "j1Render.h"
#include "p2Log.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include "j1EntityManager.h"
#include "j1Player.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Particles.h"
#include "brofiler/Brofiler/Brofiler.h"

j1FlyingEnemy::j1FlyingEnemy() :j1Entity(EntityType::FLYING_ENEMY) {

	name.create("flying_enemy");
	type = EntityType::FLYING_ENEMY;

	//copy variables from reference_enemy
	if (App->entities->reference_flying_enemy != nullptr)
	{
		speed = App->entities->reference_flying_enemy->speed;
		health = App->entities->reference_flying_enemy->health;
		damage = App->entities->reference_flying_enemy->damage;
		detection_range = App->entities->reference_flying_enemy->detection_range;
		texture = App->entities->reference_flying_enemy->texture;
		die_fx = App->entities->reference_flying_enemy->die_fx;

		animations = App->entities->reference_flying_enemy->animations;
		rest = *animations.At(0)->data;
		die = *animations.At(1)->data;
		idle = *animations.At(2)->data;

		attack = idle;
		run = idle;
		current_animation = &idle;

		player = App->entities->player_pointer;

		//colliders
		collider = App->collision->AddCollider({ 2000,200,45,26 }, COLLIDER_ENEMY, (j1Module*)this);
		raycast = App->collision->AddCollider({ 2000,200,20,2 }, COLLIDER_ENEMY, (j1Module*)this);

		score = 15;
	}

	initialPosition = position;
	lastPosition = position;
	flip = SDL_FLIP_NONE;
}

j1FlyingEnemy::~j1FlyingEnemy() {
	App->entities->DestroyEntity(this);
	App->tex->UnLoad(texture);
	texture = nullptr;
	collider->to_delete = true;
	collider = nullptr;
	raycast->to_delete = true;
	raycast = nullptr;
}

bool j1FlyingEnemy::Awake(pugi::xml_node& config) {
	bool ret = true;

	speed.x = speed.y = config.child("flying_speed").attribute("value").as_int();
	health = config.child("health").attribute("value").as_int();
	damage = config.child("damage").attribute("value").as_int();
	detection_range = config.child("detection_range").attribute("value").as_int();

	die_fx_path = config.child("die3FX").attribute("source").as_string();

	die_fx = App->audio->LoadFx(die_fx_path.GetString());


	LoadAnimations("Animations_flyingEnemy1.tmx");

	return ret;
}

bool j1FlyingEnemy::CleanUp() {
	bool ret = true;
	texture = nullptr;
	if (collider != nullptr)
	{
		collider->to_delete = true;
		collider = nullptr;
		raycast->to_delete = true;
		raycast = nullptr;
	}
	return ret;
}

bool j1FlyingEnemy::Update(float dt) {
	BROFILER_CATEGORY("FlyingEnemyUpdate", Profiler::Color::Tomato)
		bool ret = true;
	lastPosition = position;

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

		if (current_map_position.x == tile_to_go.x) {
			i++;
			if (i > 2) tile_to_go = App->map->WorldToMap(path_to_player->At(i)->x, path_to_player->At(i)->y);
		}

		//movement control
		if (current_map_position.x > tile_to_go.x) {
			//LOG("Going left");
			state = EntityState::RUN_BACKWARD;
		}
		if (current_map_position.x < tile_to_go.x) {
			//LOG("Going right");
			state = EntityState::RUN_FORWARD;
		}
		if (current_map_position.y > tile_to_go.y) {
			//LOG("Going up");
			current_speed.y = -speed.y;
		}
		if ((current_map_position.y < tile_to_go.y) && (last_collider == nullptr)) {
			//LOG("Going down");
			current_speed.y = speed.y;
		}
		if (path_to_player->Count() == 1) {
			current_speed.y = speed.y;
		}
	}

	if ((going_after_player) && (abs(player->position.x - position.x) > detection_range))
	{
		if (state == RUN_FORWARD) state = RUN_BACKWARD;
		if (state == RUN_BACKWARD) state = RUN_FORWARD;
	}

	//state machine
	switch (state)
	{
	case IDLE:
		current_animation = &idle;
		break;
	case RUN_FORWARD:
		current_animation = &run;
		current_speed.x = speed.x;
		flip = SDL_FLIP_HORIZONTAL;
		break;
	case RUN_BACKWARD:
		current_animation = &run;
		current_speed.x = -speed.x;
		flip = SDL_FLIP_NONE;
		break;
	case ATTACK:
		current_animation = &attack;
		break;
	case DIE:
		current_animation = &die;
		break;
	default:
		break;
	}

	//Movement Control
	position.x += current_speed.x * dt;
	position.y += current_speed.y * dt;

	//collider control

	if (collider != nullptr)
		collider->SetPos(position.x, position.y);
	if ((raycast != nullptr) && (collider != nullptr))
		raycast->SetPos(collider->rect.x + collider->rect.w * 0.5f - raycast->rect.w * 0.5f, position.y + current_animation->GetCurrentFrame().h - 4);

	return ret;
}



bool j1FlyingEnemy::PostUpdate() {
	BROFILER_CATEGORY("FlyingEnemyPostUpdate", Profiler::Color::Tomato)
		bool ret = true;
	App->render->Blit(texture, position.x, position.y, &current_animation->GetCurrentFrame(), flip);
	return ret;
}


void j1FlyingEnemy::OnCollision(Collider* c1, Collider* c2) {

	if (c1 == raycast)
		last_collider = c2;

	switch (c2->type)
	{
	case COLLIDER_WALL:
		position = lastPosition;
		break;

	case COLLIDER_PLAYER:
		if (!playing_fx)
		{
			App->audio->PlayFx(die_fx);
			playing_fx = true;
		}

		if (!particles_created)
		{
			App->particles->AddParticle(App->particles->dust, collider->rect.x, collider->rect.y);
			particles_created = true;
			player->score += score;
		}

		player->current_speed.y = player->enemy_bouncing;
		player->can_double_jump = true;

		if (player->state != DIE)
		{
			App->entities->DestroyEntity(this);
		}
		break;

	default:
		break;
	}
}