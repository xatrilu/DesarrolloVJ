#ifndef _jCOLLECTIBLE_H_
#define _jCOLLECTIBLE_H_
#include "j1Entity.h"

class jCollectible : public j1Entity
{
public:
	jCollectible();
	~jCollectible();

	bool Awake(pugi::xml_node& config);
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	void OnCollision(Collider* c1, Collider* c2);

private:
	j1Player* player;
};

#endif // !_j1COLLECTIBLE_H_

