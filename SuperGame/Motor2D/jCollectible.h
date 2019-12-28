#ifndef _j1COLLECTIBLE_H_
#define _j1COLLECTIBLE_H_
#include "j1Entity.h"

class j1Collectible : public j1Entity
{
public:
	j1Collectible();
	~j1Collectible();

	bool Awake(pugi::xml_node& config);
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	void OnCollision(Collider* c1, Collider* c2);

private:
	j1Player* player;
};

#endif 

