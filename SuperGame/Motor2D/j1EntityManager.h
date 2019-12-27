#ifndef _ENTITY_MANAGER_H_
#define _ENTITY_MANAGER_H_

#include "j1Module.h"
#include "p2Point.h"
#include "Animation.h"
#include "SDL_image/include/SDL_image.h"

class j1Entity;
struct SDL_Texture;
class j1Player;
class j1WalkingEnemy;
class j1WalkingEnemy2;
class j1FlyingEnemy;
class j1Collectible;

enum class EntityType
{
	PLAYER,
	WALKING_ENEMY,
	FLYING_ENEMY,
	COLLECTIBLE,
	UNKNOWN
};


class j1EntityManager : public j1Module
{
public:

	j1EntityManager();
	~j1EntityManager();

	virtual bool Awake(pugi::xml_node&);
	virtual bool Start();

	virtual bool PreUpdate();
	virtual bool Update(float dt);
	virtual bool PostUpdate();

	virtual bool CleanUp();

	bool Load(pugi::xml_node& data);
	bool Save(pugi::xml_node& data) const;
	bool CheckpointSave();
	bool CheckpointLoad();

	j1Entity* getPlayer();
	j1Entity* CreateEntity(EntityType type, int position_x, int position_y);
	virtual void DestroyEntity(j1Entity* delete_entity);
	void		 DestroyAllEntities();
	void		 RellocateEntities();

public:

	p2List<j1Entity*> entities;
	pugi::xml_node config_data;

	j1Player* player_pointer;

public:
	int gravity = 0;
	int max_falling_speed = 0;

	j1Player* reference_player;
	j1WalkingEnemy* reference_walking_enemy;
	j1FlyingEnemy* reference_flying_enemy;
	j1Collectible* reference_collectible;

	//sfx
	uint walking_enemy_attack_fx;
	uint flying_enemy_attack_fx;
	uint walking_enemy2_attack_fx;

	uint walking_enemy_die_fx;
	uint flying_enemy_die_fx;
	uint walking_enemy2_die_fx;

	float time_between_updates;
	float accumulated_time;

	bool blocked_movement;

};

#endif // !_ENTITY_MANAGER_H_

