#pragma once
#ifndef _j1WALKING_ENEMY2_H
#define _j1WALKING_ENEMY2_H
#include "j1Entity.h"
#include "j1Audio.h"

class j1WalkingEnemy2 : public j1Entity
{
public:
	j1WalkingEnemy2();
	virtual ~j1WalkingEnemy2();

	bool Awake(pugi::xml_node& config);
	bool Update(float dt);
	bool PostUpdate();

	void OnCollision(Collider* c1, Collider* c2);

	void MovementControl(float dt) {}

	bool Save(pugi::xml_node& data) const { return true; }
	bool Load(pugi::xml_node& data) { return true; }

private:
	float attacking_range = 1;

};

#endif // !_j1WALKING_ENEMY_H
