#include "jPickUpObject.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1Player.h"
#include "j1Textures.h"

jPickUpObject::jPickUpObject() : j1Entity(EntityType::COLLECTIBLE) {

	name.create("collectible");
	type = EntityType::COLLECTIBLE;
	LoadAnimations("coin_animation.tmx");

	if (App->entities->reference_collectible != nullptr)
	{
		texture = App->entities->reference_collectible->texture;
		die_fx = App->entities->reference_collectible->die_fx;
		collider = App->collision->AddCollider({ 1000, 1000, 32,32 }, COLLIDER_COLLECTIBLE, this);
		player = App->entities->player_pointer;
		score = 25;
	}
	current_animation = &idle;
}

jPickUpObject::~jPickUpObject() {}

bool jPickUpObject::Awake(pugi::xml_node& config) {
	bool ret = true;
	die_fx_path = config.child("coin_sound").attribute("source").as_string();
	die_fx = App->audio->LoadFx(die_fx_path.GetString());
	return true;
}

bool jPickUpObject::Update(float dt) {
	bool ret = true;
	collider->SetPos(position.x, position.y);
	return ret;
}

bool jPickUpObject::PostUpdate() {
	bool ret = true;
	ret = App->render->Blit(texture, position.x, position.y, &current_animation->GetCurrentFrame());
	return ret;
}

bool jPickUpObject::CleanUp() {
	bool ret = true;
	texture = nullptr;
	if (collider != nullptr) {
		collider->to_delete = true;
		collider = nullptr;
	}
	return ret;
}

void jPickUpObject::OnCollision(Collider* c1, Collider* c2) {

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