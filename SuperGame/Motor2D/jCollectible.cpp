#include "jCollectible.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1Player.h"
#include "j1Textures.h"

jCollectible::jCollectible() : j1Entity(EntityType::COLLECTIBLE) {

	name.create("collectible");
	type = EntityType::COLLECTIBLE;
	LoadAnimations("coin_animation.tmx");

	if (App->entities->reference_collectible != nullptr)
	{
		texture = App->entities->reference_collectible->texture;
		collider = App->collision->AddCollider({ position.x, position.y, 32,32 }, COLLIDER_COLLECTIBLE, this);
		player = App->entities->player_pointer;
		score = 20;
	}
	current_animation = &idle;
}

jCollectible::~jCollectible() {}

bool jCollectible::Awake(pugi::xml_node& config) {
	bool ret = true;
	return true;
}

bool jCollectible::Update(float dt) {
	bool ret = true;
	collider->SetPos(position.x, position.y);
	return ret;
}

bool jCollectible::PostUpdate() {
	bool ret = true;
	ret = App->render->Blit(texture, position.x, position.y, &current_animation->GetCurrentFrame());
	return ret;
}

bool jCollectible::CleanUp() {
	bool ret = true;
	texture = nullptr;
	if (collider != nullptr) {
		collider->to_delete = true;
		collider = nullptr;
	}
	return ret;
}

void jCollectible::OnCollision(Collider* c1, Collider* c2) {

	switch (c2->type)
	{
	case COLLIDER_PLAYER:
		player->score += score;
		App->entities->DestroyEntity(this);
		break;

	default:
		break;
	}
}