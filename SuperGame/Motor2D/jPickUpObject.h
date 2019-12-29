#ifndef _JPICKUPOBECT_H_
#define _JPICKUPOBECT_H_
#include "j1Entity.h"

class jPickUpObject : public j1Entity
{
public:
	jPickUpObject();
	~jPickUpObject();

	bool Awake(pugi::xml_node& config);
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	void OnCollision(Collider* c1, Collider* c2);

private:
	j1Player* player;
};

#endif 

