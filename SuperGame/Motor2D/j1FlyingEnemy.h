#ifndef _j1FLYING_ENEMY_H
#define _j1FLYING_ENEMY_H
#include "j1Entity.h"
#include "p2DynArray.h"

class j1FlyingEnemy :public j1Entity
{
public:
	j1FlyingEnemy();
	virtual ~j1FlyingEnemy();

	bool Awake(pugi::xml_node& config);
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void OnCollision(Collider* c1, Collider* c2);

	bool Save(pugi::xml_node& data) const { return true; }
	bool Load(pugi::xml_node& data) { return true; }
private:
	j1Player* player;
};

#endif 