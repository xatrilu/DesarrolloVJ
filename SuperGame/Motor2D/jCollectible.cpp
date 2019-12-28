#include "jCollectible.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1Player.h"
#include "j1Textures.h"

j1Collectible::j1Collectible() : j1Entity(EntityType::COLLECTIBLE) {

	name.create("collectible");
	type = EntityType::COLLECTIBLE;
	LoadAnimations("coin_animation.tmx");

	if (App->entities->reference_collectible != nullptr)
	{
		texture = App->entities->reference_collectible->texture;
		die_fx = App->entities->reference_collectible->die_fx;
		collider = App->collision->AddCollider({ position.x, position.y, 32,32 }, COLLIDER_COLLECTIBLE, this);
		player = App->entities->player_pointer;
		score = 20;
	}
	current_animation = &idle;
}

j1Collectible::~j1Collectible() {}

bool j1Collectible::Awake(pugi::xml_node& config) {
	bool ret = true;
	die_fx_path = config.child("coin_sound").attribute("source").as_string();
	die_fx = App->audio->LoadFx(die_fx_path.GetString());
	return true;
}

bool j1Collectible::Update(float dt) {
	bool ret = true;
	collider->SetPos(position.x, position.y);
	return ret;
}

bool j1Collectible::PostUpdate() {
	bool ret = true;
	ret = App->render->Blit(texture, position.x, position.y, &current_animation->GetCurrentFrame());
	return ret;
}

bool j1Collectible::CleanUp() {
	bool ret = true;
	texture = nullptr;
	if (collider != nullptr) {
		collider->to_delete = true;
		collider = nullptr;
	}
	return ret;
}

void j1Collectible::OnCollision(Collider* c1, Collider* c2) {

	switch (c2->type)
	{
	case COLLIDER_PLAYER:
		player->score += score;
		App->audio->PlayFx(die_fx);
		App->entities->DestroyEntity(this);
		break;

	default:
		break;
	}
}