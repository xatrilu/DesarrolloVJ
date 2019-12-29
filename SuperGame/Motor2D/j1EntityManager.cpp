#include "j1EntityManager.h"
#include "j1App.h"
#include "j1Player.h"
#include "p2Log.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Collision.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Audio.h"
#include "j1Particles.h"
#include "j1WalkingEnemy.h"
#include "j1FlyingEnemy.h"
#include "jCollectible.h"
#include "brofiler/Brofiler/Brofiler.h"


j1EntityManager::j1EntityManager() {
	name.create("entities");
	reference_player = nullptr;
	reference_walking_enemy = nullptr;
	reference_flying_enemy = nullptr;
	reference_collectible = nullptr;

	blocked_movement = false;

	time_between_updates = 0.01f;
	accumulated_time = 0;
}


j1EntityManager::~j1EntityManager() {
}

j1Entity* j1EntityManager::CreateEntity(EntityType type, int position_x, int position_y) {
	BROFILER_CATEGORY("EntityCreation", Profiler::Color::Linen)
		//static_assert(EntityType::UNKNOWN == 4, "code needs update");
		j1Entity* entity = nullptr;
	switch (type)
	{
	case EntityType::PLAYER:
		entity = new j1Player();
		break;
	case EntityType::WALKING_ENEMY:
		entity = new j1WalkingEnemy();
		break;
	case EntityType::FLYING_ENEMY:
		entity = new j1FlyingEnemy();
		break;
	case EntityType::COLLECTIBLE:
		entity = new j1Collectible();
		break;
	case EntityType::UNKNOWN:
		break;
	default:
		break;
	}

	entity->position.x = entity->initialPosition.x = position_x;
	entity->position.y = entity->initialPosition.y = position_y;

	if (entity != nullptr) entities.add(entity);

	return entity;
}

void j1EntityManager::DestroyEntity(j1Entity* entity) {
	BROFILER_CATEGORY("EntityDestruction", Profiler::Color::Orange)
		p2List_item<j1Entity*>* item;

	if (entity != nullptr) {
		item = entities.At(entities.find(entity));
		entity->CleanUp();
		entities.del(item);;
	}
}

void j1EntityManager::DestroyAllEntities() {
	p2List_item<j1Entity*>* item;

	for (item = entities.start; item != nullptr; item = item->next)
	{
		DestroyEntity(item->data);
	}
	DestroyEntity(player_pointer);
}

bool j1EntityManager::Awake(pugi::xml_node& config) {
	bool ret = true;

	config_data = config;

	gravity = config.child("gravity").attribute("value").as_int();
	max_falling_speed = config.child("max_falling_speed").attribute("value").as_int();

	//player creation
	reference_player = new j1Player();
	reference_player->Awake(config.child("player"));
	//entities.add(player);

	//reference walking enemy
	reference_walking_enemy = new j1WalkingEnemy();
	reference_walking_enemy->Awake(config.child("walking_enemy"));

	//reference flying enemy
	reference_flying_enemy = new j1FlyingEnemy();
	reference_flying_enemy->Awake(config.child("flying_enemy"));

	//reference collectible
	reference_collectible = new j1Collectible();
	reference_collectible->Awake(config.child("collectible"));

	return ret;
}

bool j1EntityManager::Start()
{
	bool ret = true;

	//player->Start();
	reference_player->texture = App->tex->Load("sprites/characters/spritesheet_traveler2.png");
	reference_walking_enemy->texture = App->tex->Load("sprites/characters/sheet_hero_idle.png");
	reference_flying_enemy->texture = App->tex->Load("sprites/characters/Sprite_bat.png");
	reference_collectible->texture = App->tex->Load("sprites/characters/coin.png");

	for (p2List_item<j1Entity*>* entity = entities.start; entity != nullptr; entity = entity->next)
	{
		if (entity->data->type == EntityType::PLAYER) {
			entity->data->texture = reference_player->texture;
			//player_pointer = (j1Player*)entity->data;
		}
		if (entity->data->type == EntityType::WALKING_ENEMY) {
			entity->data->texture = reference_walking_enemy->texture;
		}
		if (entity->data->type == EntityType::FLYING_ENEMY) {
			entity->data->texture = reference_flying_enemy->texture;
		}
		if (entity->data->type == EntityType::COLLECTIBLE) {
			entity->data->texture = reference_collectible->texture;
		}
	}

	return ret;
}

bool j1EntityManager::CleanUp()
{
	bool ret = true;

	App->tex->UnLoad(reference_player->texture);
	reference_player->texture = nullptr;

	App->tex->UnLoad(reference_walking_enemy->texture);
	reference_walking_enemy->texture = nullptr;

	App->tex->UnLoad(reference_flying_enemy->texture);
	reference_flying_enemy->texture = nullptr;

	App->tex->UnLoad(reference_collectible->texture);
	reference_collectible->texture = nullptr;

	//destroy all entities
	for (p2List_item<j1Entity*>* entity = entities.start; entity != nullptr; entity = entity->next)
	{
		entity->data->DestroyEntity(entity->data);
	}

	return ret;
}

j1Entity* j1EntityManager::getPlayer() {
	for (p2List_item<j1Entity*>* item = entities.start; item != nullptr; item = item->next)
	{
		if (item->data == player_pointer) return item->data;
	}
}

bool j1EntityManager::PreUpdate()
{
	BROFILER_CATEGORY("EntitiesPreUpdate", Profiler::Color::Bisque)
		bool ret = true;
	if (player_pointer != nullptr)
	{
		player_pointer->PreUpdate();
	}
	return ret;
}

bool j1EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("EntitiesUpdate", Profiler::Color::Bisque)
		bool ret = true;

	if ((!App->pause) && (!blocked_movement)) {
		for (p2List_item<j1Entity*>* entity = entities.start; entity != nullptr; entity = entity->next)
		{
			entity->data->Update(dt);
		}
	}

	//accumulated_time += dt;

	return ret;
}

bool j1EntityManager::PostUpdate()
{
	BROFILER_CATEGORY("EntitiesPostUpdate", Profiler::Color::Bisque)
		bool ret = true;
	for (p2List_item<j1Entity*>* entity = entities.start; entity != nullptr; entity = entity->next)
	{
		entity->data->PostUpdate();
	}
	return ret;
}

void j1EntityManager::RellocateEntities() {
	for (p2List_item<j1Entity*>* entity = entities.start; entity != nullptr; entity = entity->next)
	{
		entity->data->position = entity->data->initialPosition;
		entity->data->going_after_player = false;
	}
}

bool j1EntityManager::Load(pugi::xml_node& data)
{
	bool ret = true;

	pugi::xml_node entity_node = data.first_child();

	DestroyAllEntities();

	while (entity_node != nullptr)
	{
		p2SString entity_name(entity_node.name());
		int x_position = entity_node.attribute("position_x").as_int();
		int y_position = entity_node.attribute("position_y").as_int();

		if (entity_name == "player") {
			player_pointer = (j1Player*)CreateEntity(EntityType::PLAYER, x_position, y_position);
			player_pointer->score = entity_node.attribute("score").as_int();
			player_pointer->current_speed = { 0,0 };
		}

		if (entity_name == "walking_enemy")
			CreateEntity(EntityType::WALKING_ENEMY, x_position, y_position);

		if (entity_name == "flying_enemy")
			CreateEntity(EntityType::FLYING_ENEMY, x_position, y_position);

		if (entity_name == "collectible")
			CreateEntity(EntityType::COLLECTIBLE, x_position, y_position);

		entity_node = entity_node.next_sibling();
	}

	return ret;
}

bool j1EntityManager::Save(pugi::xml_node& data) const
{
	bool ret = true;
	p2List_item<j1Entity*>* item;

	for (item = entities.start; item != nullptr; item = item->next)
	{
		pugi::xml_node child = data.append_child(item->data->name.GetString());
		child.append_attribute("position_x") = item->data->position.x;
		child.append_attribute("position_y") = item->data->position.y;
		if (item->data->type == EntityType::PLAYER)
		{
			child.append_attribute("score") = player_pointer->score;
		}
	}

	return ret;
}

bool j1EntityManager::CheckpointSave() {
	bool ret = true;
	LOG("Checkpoint triggered");

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;
	pugi::xml_node entities_node;

	root = data.append_child("Checkpoint");
	ret = App->render->Save(root.append_child("render"));
	entities_node = root.append_child("entities");

	p2List_item<j1Entity*>* entity;
	for (entity = entities.start; entity != nullptr; entity = entity->next)
	{
		pugi::xml_node child = entities_node.append_child(entity->data->name.GetString());
		if (entity->data == player_pointer)
		{
			child.append_attribute("position_x") = entity->data->position.x;
			child.append_attribute("position_y") = entity->data->position.y;
			child.append_attribute("score") = player_pointer->score;
		}
		else
		{
			child.append_attribute("position_x") = entity->data->initialPosition.x;
			child.append_attribute("position_y") = entity->data->initialPosition.y;
		}
	}

	if (ret == true)
	{
		data.save_file(App->checkpoint_save.GetString());
		LOG("... finished saving", );
	}

	data.reset();
	return ret;
}

bool j1EntityManager::CheckpointLoad()
{
	bool ret = true;

	pugi::xml_document doc;
	pugi::xml_node root;

	DestroyAllEntities();

	pugi::xml_parse_result result = doc.load_file(App->checkpoint_save.GetString());

	if (result == NULL) LOG("Error loading checkpoint data from %s", App->checkpoint_save.GetString());

	else {
		root = doc.first_child();
		App->render->camera.x = root.child("render").child("camera").attribute("x").as_int();
		App->render->camera.y = root.child("render").child("camera").attribute("y").as_int();

		pugi::xml_node entity_node = root.child("entities").first_child();

		while (entity_node != nullptr)
		{
			p2SString entity_name(entity_node.name());
			int x_position = entity_node.attribute("position_x").as_int();
			int y_position = entity_node.attribute("position_y").as_int();

			if (entity_name == "player") {
				CreateEntity(EntityType::PLAYER, x_position, y_position);
				player_pointer->score = entity_node.attribute("score").as_int();
			}

			if (entity_name == "walking_enemy")
				CreateEntity(EntityType::WALKING_ENEMY, x_position, y_position);

			if (entity_name == "flying_enemy")
				CreateEntity(EntityType::FLYING_ENEMY, x_position, y_position);

			if (entity_name == "collectible")
				CreateEntity(EntityType::COLLECTIBLE, x_position, y_position);

			entity_node = entity_node.next_sibling();
		}
	}
	return ret;
}