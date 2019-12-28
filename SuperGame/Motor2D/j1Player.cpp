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
#include "j1Particles.h"
#include "j1EntityManager.h"
#include "j1FadeToBlack.h"
#include "Console.h"
#include "ConsoleCommands.h"
#include "brofiler/Brofiler/Brofiler.h"

j1Player::j1Player() :j1Entity(EntityType::PLAYER) {

	name.create("player");
	LoadAnimations("Animations_traveller.tmx");
	current_animation = &idle;
	crouch_down.loop = false;
	crouch_down.loop = false;

	App->entities->player_pointer = this;

	gravity = App->entities->gravity;
	max_falling_speed = App->entities->max_falling_speed;

	type = EntityType::PLAYER;

	particles_created = false;
	controls_blocked = false;
	isVisible = true;
	god = false;

	can_double_jump = true;
	can_go_left = true;
	can_go_right = true;
	lives = 3;
	score = 0;

	if (App->entities->reference_player != nullptr)
	{
		texture = App->entities->reference_player->texture;
		max_running_speed = App->entities->reference_player->max_running_speed;
		max_side_speed = App->entities->reference_player->max_side_speed;
		acceleration = App->entities->reference_player->acceleration;
		deceleration = App->entities->reference_player->deceleration;

		jumpImpulse = App->entities->reference_player->jumpImpulse;
		doubleJumpImpulse = App->entities->reference_player->doubleJumpImpulse;
		enemy_bouncing = App->entities->reference_player->enemy_bouncing;

		jump_fx = App->entities->reference_player->jump_fx;
		double_Jump_fx = App->entities->reference_player->double_Jump_fx;
		die_fx = App->entities->reference_player->die_fx;

		position = App->entities->reference_player->initialPosition;

		collider = App->collision->AddCollider(SDL_Rect{ initialPosition.x, initialPosition.y,32,64 }, COLLIDER_PLAYER, (j1Module*)this);
		raycast = App->collision->AddCollider(SDL_Rect{ initialPosition.x, initialPosition.y,20,5 }, COLLIDER_PLAYER_ATTACK, (j1Module*)this);

		isVisible = true;
		grounded = false;

		current_speed = { 0,0 };
		last_score = 0;

		App->console->CreateCommand("god_mode", this, "Toggles god mode ON/OFF");
	}
}

j1Player::~j1Player() { }

bool j1Player::Awake(pugi::xml_node& config) {

	bool ret = true;
	LOG("Loading Player Data");

	folder.create(config.child("folder").child_value());

	current_animation = &idle;

	gravity = App->entities->gravity;
	max_falling_speed = App->entities->max_falling_speed;

	//set initial attributes of the player
	max_running_speed = config.child("max_running_speed").attribute("value").as_float();
	max_side_speed = config.child("max_side_speed").attribute("value").as_float();
	acceleration = config.child("acceleration").attribute("value").as_float();
	deceleration = config.child("deceleration").attribute("value").as_float();

	jumpImpulse = config.child("jumpImpulse").attribute("value").as_float();
	doubleJumpImpulse = config.child("doubleJumpImpulse").attribute("value").as_float();
	enemy_bouncing = config.child("enemy_bouncing").attribute("value").as_int();

	//player fx
	die_fx_path = config.child("dieFX").attribute("source").as_string();
	jump_fx_path = config.child("jumpFX").attribute("source").as_string();
	double_Jump_fx_path = config.child("jump2FX").attribute("source").as_string();

	return ret;
}

bool j1Player::Start() {

	current_speed = { 0,0 };

	die_fx = App->audio->LoadFx(die_fx_path.GetString());
	jump_fx = App->audio->LoadFx(jump_fx_path.GetString());
	double_Jump_fx = App->audio->LoadFx(double_Jump_fx_path.GetString());

	return true;
}

bool j1Player::CleanUp() {
	texture = nullptr;
	if (collider != nullptr)
	{
		collider->to_delete = true;
		collider = nullptr;
	}
	if (raycast != nullptr)
	{
		raycast->to_delete = true;
		raycast = nullptr;
	}
	return true;
}


bool j1Player::PreUpdate() {
	BROFILER_CATEGORY("PlayerPreUpdate", Profiler::Color::CadetBlue)

		//get player input
		player_input.pressing_W = App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT;
	player_input.pressing_A = App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT;
	player_input.pressing_S = App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT;
	player_input.pressing_D = App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT;
	player_input.pressing_F = App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT;
	player_input.pressing_space = App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN;

	lastPosition = position;
	last_state = state;

	//LOG("State %s", state);

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		god = !god;
		if (god)
		{
			state = IDLE;
			current_speed.y = 0;
		}
	}

	if (!App->pause)
	{
		if (!controls_blocked) {
			if (lives > 0) {
				if (state == IDLE)
				{
					can_double_jump = true;

					if (player_input.pressing_D) state = RUN_FORWARD;

					if (player_input.pressing_A) state = RUN_BACKWARD;

					if (player_input.pressing_S) state = CROUCH_DOWN;

					if (player_input.pressing_F)
					{
						if (flip == SDL_FLIP_NONE)
						{
							state = SLIDE_FORWARD;
						}
						else
						{
							state = SLIDE_BACKWARD;
						}
					}

					if ((player_input.pressing_space) && (!god))
					{
						App->audio->PlayFx(jump_fx);
						state = JUMP;
						current_speed.y = jumpImpulse;
						grounded = false;
					}

				}

				if (state == RUN_FORWARD)
				{
					if (player_input.pressing_D)
					{
						if (current_speed.x < max_running_speed)
						{
							current_speed.x += acceleration;
						}
					}

					if (!player_input.pressing_D) state = IDLE;

					if ((player_input.pressing_space) && (!god))
					{
						App->audio->PlayFx(jump_fx);
						state = JUMP;
						current_speed.y = jumpImpulse;
						grounded = false;
					}

					if (player_input.pressing_F) state = SLIDE_FORWARD;
				}

				if (state == RUN_BACKWARD)
				{
					if (player_input.pressing_A)
					{
						if (current_speed.x > -max_running_speed)
						{
							current_speed.x -= acceleration;
						}
					}

					if (!player_input.pressing_A) state = IDLE;

					if ((player_input.pressing_space) && (!god))
					{
						App->audio->PlayFx(jump_fx);
						state = JUMP;
						current_speed.y = jumpImpulse;
						grounded = false;
					}

					if (player_input.pressing_F) state = SLIDE_BACKWARD;
				}


				if (state == CROUCH_DOWN)
				{
					if (!player_input.pressing_S)
					{
						state = CROUCH_UP;
						crouch_down.Reset();
					}
				}

				if (state == CROUCH_UP)
				{
					if (current_animation->Finished()) {
						state = IDLE;
						crouch_up.Reset();
					}
				}

				if (state == SLIDE_FORWARD)
				{
					if (!player_input.pressing_F) state = IDLE;
				}

				if (state == SLIDE_BACKWARD)
				{
					if (!player_input.pressing_F) state = IDLE;
				}

				if (state == JUMP)
				{
					if ((player_input.pressing_D) && (current_speed.x < max_side_speed)) current_speed.x += acceleration;
					if ((player_input.pressing_A) && (current_speed.x > -max_side_speed)) current_speed.x -= acceleration;

					//double jump
					if ((player_input.pressing_space) && (can_double_jump == true) && (current_speed.y <= jumpImpulse * 0.5f))
					{
						App->audio->PlayFx(double_Jump_fx);
						jump.Reset();
						current_speed.y = doubleJumpImpulse;
						can_double_jump = false;
						App->particles->AddParticle(App->particles->dust, position.x, position.y + current_animation->GetCurrentFrame().h * 0.75f, COLLIDER_NONE, 0, flip);
					}

					if (current_animation->Finished())
					{
						state = FALL;
						jump.Reset();
					}

				}
				if (state == FALL)
				{
					if ((player_input.pressing_D) && (can_go_right == true) && (current_speed.x < max_side_speed)) current_speed.x += acceleration;
					if ((player_input.pressing_A) && (can_go_left == true) && (current_speed.x > max_side_speed)) current_speed.x -= acceleration;

					if ((player_input.pressing_space) && (can_double_jump == true) & (current_speed.y <= jumpImpulse * 0.5f))
					{
						jump.Reset();
						state = JUMP;
						App->audio->PlayFx(double_Jump_fx);
						current_speed.y = doubleJumpImpulse;
						can_double_jump = false;
						grounded = false;
						App->particles->AddParticle(App->particles->dust, position.x, position.y + current_animation->GetCurrentFrame().h * 0.75f, COLLIDER_NONE, 0, flip);
					}

					if (current_animation->Finished()) fall.Reset();
				}
			}
			else {
				App->scene->GameOver();
			}
		}
	}

	return true;
}

bool j1Player::Update(float dt) {
	BROFILER_CATEGORY("PlayerUpdate", Profiler::Color::CadetBlue)

		MovementControl(dt); //calculate new position

		//collider->SetPos(floor(position.x), position.y + 6);

	if (last_collider != nullptr)
	{
		if ((!raycast->CheckCollision(last_collider->rect)) && (state != DIE))
		{
			grounded = false;
			if ((state != JUMP) && (!god))
				state = FALL;
		}
	}

	switch (state)
	{
	case IDLE:
		current_animation = &idle;
		/*collider->SetSize(32, 56);
		if (flip == SDL_FLIP_NONE)
			collider->SetPos(position.x, position.y + 8);
		else
			collider->SetPos(position.x + 32, position.y + 8);*/
		break;

	case RUN_FORWARD:
		current_animation = &run;
		flip = SDL_FLIP_NONE;
		/*collider->SetSize(46, 54);
			collider->SetPos(position.x, position.y + 10);*/
		break;

	case RUN_BACKWARD:
		current_animation = &run;
		flip = SDL_FLIP_HORIZONTAL;
		/*collider->SetSize(46, 54);
		collider->SetPos(position.x +18, position.y + 10);*/
		break;

	case CROUCH_DOWN:
		current_animation = &crouch_down;
		//collider->SetSize(32, 32);
		break;

	case CROUCH_UP:
		current_animation = &crouch_up;
		//collider->SetSize(32, 32);
		break;

	case SLIDE_FORWARD:
		current_animation = &slide;
		flip = SDL_FLIP_NONE;
		/*collider->SetSize(52, 42);
		collider->SetPos(position.x, position.y + 22);*/
		break;

	case SLIDE_BACKWARD:
		current_animation = &slide;
		flip = SDL_FLIP_HORIZONTAL;
		/*collider->SetSize(52, 42);
		collider->SetPos(position.x  + 12, position.y + 22);*/
		break;

	case JUMP:
		current_animation = &jump;

		if (current_speed.y <= 0) {
			state = FALL;
			jump.Reset();
		}
		if ((last_state = RUN_FORWARD) || (last_state == RUN_BACKWARD)) {
			current_speed.x *= 0.5f;
		}
		/*
		collider->SetSize(32, 64);
		if (flip == SDL_FLIP_NONE)
			collider->SetPos(position.x, position.y);
		else
			collider->SetPos(position.x + 32, position.y);*/
		break;
	case FALL:
		current_animation = &fall;
		/*collider->SetSize(50, 64);
		if (flip == SDL_FLIP_NONE)
			collider->SetPos(position.x, position.y + 6);
		else
			collider->SetPos(position.x + 14, position.y + 6);*/
		break;
	case DIE:
		if (!god) {

		}
		break;
	default:
		LOG("No state found");
		break;
	}



	if (flip == SDL_FLIP_NONE) collider->SetPos(position.x + 8, position.y);
	else collider->SetPos(position.x + 16, position.y);

	raycast->SetPos(floor(collider->rect.x + collider->rect.w * 0.5f - raycast->rect.w * 0.5f), collider->rect.y + collider->rect.h);

	return true;
}

bool j1Player::PostUpdate() {
	BROFILER_CATEGORY("PlayerPreUpdate", Profiler::Color::CadetBlue)
		if (isVisible)
		{
			App->render->Blit(texture, position.x, position.y, &current_animation->GetCurrentFrame(), flip);
		}
	return true;
}

//control over all player movement physics
void j1Player::MovementControl(float dt) {

	//normal mode
	if (!god)
	{
		if (!grounded) {
			if (current_speed.y > max_falling_speed) current_speed.y -= gravity * dt;
			position.y -= current_speed.y * dt;
		}

		//deceleration
		if ((!player_input.pressing_D) && (current_speed.x > 0)) current_speed.x -= deceleration * dt;
		if ((!player_input.pressing_A) && (current_speed.x < 0)) current_speed.x += deceleration * dt;
		if ((floor(current_speed.x) <= 30) && (floor(current_speed.x) >= -30)) current_speed.x = 0;

		position.x += current_speed.x * dt;
	}
	else
	{
		//god mode
		if ((!player_input.pressing_A) && (!player_input.pressing_D))current_speed.x = 0;
		if ((!player_input.pressing_W) && ((!player_input.pressing_W))) current_speed.y = 0;

		if (player_input.pressing_W) current_speed.y = -jumpImpulse;
		if (player_input.pressing_S) current_speed.y = jumpImpulse;

		if (current_speed.x > 0) state = RUN_FORWARD;
		if (current_speed.x < 0) state = RUN_BACKWARD;

		position.x += current_speed.x * dt;
		position.y += current_speed.y * dt;
	}


}

bool j1Player::Save(pugi::xml_node& data) const {

	pugi::xml_node p_position = data.append_child("position");

	p_position.append_attribute("x") = position.x;
	p_position.append_attribute("y") = position.y;

	return true;
}

bool j1Player::Load(pugi::xml_node& data)
{
	position.x = data.child("position").attribute("x").as_int();
	position.y = data.child("position").attribute("y").as_int();

	return true;
}

void j1Player::OnCollision(Collider* c1, Collider* c2) {

	if (c1 == raycast)
	{
		last_collider = c2;
	}

	if (!god)
	{
		switch (c2->type)
		{
		case COLLIDER_WALL:
			if (position.y + current_animation->GetCurrentFrame().h < c2->rect.y + COLLIDER_MARGIN) {
				grounded = true;
				position.y = c2->rect.y - current_animation->GetCurrentFrame().h;
				current_speed.y = 0;
				if (state != DIE) state = IDLE;
			}

			if (position.y + current_animation->GetCurrentFrame().h > c2->rect.y) {
				position.x = lastPosition.x;
			}
			if (position.y > c2->rect.y + c2->rect.h - COLLIDER_MARGIN) {
				position.y = c2->rect.y + c2->rect.h;
				current_speed.y = 0;
			}
			if ((position.y > c2->rect.y) && (position.x > c2->rect.x) && (position.x + current_animation->GetCurrentFrame().w < c2->rect.x + c2->rect.w) && (position.x < c2->rect.x + c2->rect.w)) {
				position.y = lastPosition.y;
				if (lastPosition.y + current_animation->GetCurrentFrame().h > c2->rect.y) {
					//position.y = c2->rect.y - current_animation->GetCurrentFrame().h;
				}

			}
			break;
		case COLLIDER_PLATFORM:
			if ((c1->rect.y + c1->rect.h < c2->rect.y) || ((position.y + current_animation->GetCurrentFrame().h * 0.7 < c2->rect.y) && (lastPosition.y < position.y)))
			{
				grounded = true;
				position.y = c2->rect.y - current_animation->GetCurrentFrame().h;
				current_speed.y = 0;
				state = IDLE;
				last_collider = c2;
			}
			if (grounded)
			{
				if (position.y + current_animation->GetCurrentFrame().h * 0.5f > c2->rect.y)
				{
					if (position.x + current_animation->GetCurrentFrame().w < c2->rect.x + COLLIDER_MARGIN) position.x = c2->rect.x - current_animation->GetCurrentFrame().w;
					if (position.x > c2->rect.x + c2->rect.w - COLLIDER_MARGIN) position.x = c2->rect.x + c2->rect.w;
				}
			}
			break;
		case TRIGGER:
			if (c2->level_change) {
				if (App->scene->current_level == LEVEL_1) App->fade_to_black->FadeToBlack(LEVEL_1, LEVEL_2);
				if (App->scene->current_level == LEVEL_2) App->fade_to_black->FadeToBlack(LEVEL_2, LEVEL_1);
				App->entities->blocked_movement = true;
			}

			if (c2->isCheckpoint) {
				if (!c2->checkpoint_tiggered) {
					App->entities->CheckpointSave();
					last_checkpoint = c2;
				}
				c2->checkpoint_tiggered = true;
			}

			c2->to_delete = true;
			c2 = nullptr;
			break;

		case COLLIDER_DEATH:
			if (!particles_created) {
				App->particles->AddParticle(App->particles->dust, position.x - 10, position.y, COLLIDER_NONE, 0, flip);
				App->particles->AddParticle(App->particles->dust, position.x, position.y + 20, COLLIDER_NONE, 0, flip);
				App->particles->AddParticle(App->particles->dust, position.x - 10, position.y + current_animation->GetCurrentFrame().h - 22, COLLIDER_NONE, 0, flip);
				App->particles->AddParticle(App->particles->dust, position.x + 2, position.y + current_animation->GetCurrentFrame().h - 2, COLLIDER_NONE, 0, flip);
				isVisible = false;
				App->audio->PlayFx(die_fx);
				state = IDLE;
				current_speed.x = current_speed.y = 0;
				App->fade_to_black->FadeToBlack(App->scene->current_level, App->scene->current_level);
				particles_created = true;
				position = initialPosition;
				lives--;
				score = last_score;
			}
			break;

		case COLLIDER_ENEMY:
			if (state != FALL) {
				if (!particles_created) {
					App->particles->AddParticle(App->particles->dust, position.x - 10, position.y, COLLIDER_NONE, 0, flip);
					App->particles->AddParticle(App->particles->dust, position.x, position.y + 20, COLLIDER_NONE, 0, flip);
					App->particles->AddParticle(App->particles->dust, position.x - 10, position.y + current_animation->GetCurrentFrame().h - 22, COLLIDER_NONE, 0, flip);
					App->particles->AddParticle(App->particles->dust, position.x + 2, position.y + current_animation->GetCurrentFrame().h - 2, COLLIDER_NONE, 0, flip);
					isVisible = false;
					App->audio->PlayFx(die_fx);
					state = IDLE;
					current_speed.x = current_speed.y = 0;
					App->fade_to_black->FadeToBlack(App->scene->current_level, App->scene->current_level);
					particles_created = true;
					position = initialPosition;
					lives--;
					score = last_score;
				}
			}
			break;
		default:
			break;
		}
	}
}

void j1Player::OnCommand(p2SString command) {

	if (command == "god_mode")
	{
		god = !god;

		if (god)
			LOG("God mode activated");

		if (!god)
			LOG("God mode deactivated");
	}
}
