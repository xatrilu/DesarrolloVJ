#ifndef _j1WALKING_ENEMY_H
#define _j1WALKING_ENEMY_H
#include "j1Entity.h"
#include "j1Audio.h"

class j1WalkingEnemy : public j1Entity
{
public:
	j1WalkingEnemy();
	virtual ~j1WalkingEnemy();

	bool Awake(pugi::xml_node& config);
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void OnCollision(Collider* c1, Collider* c2);

	void MovementControl(float dt) {}

	bool Save(pugi::xml_node& data) const { return true; }
	bool Load(pugi::xml_node& data) { return true; }

private:
	float attacking_range = 1;
	j1Player* player;
};

#endif 
