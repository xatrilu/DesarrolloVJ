#ifndef _j1PLAYER_H_
#define _j1PLAYER_H_

#include "j1Audio.h"
#include "p2Vec2.h"
#include "j1Entity.h"

struct Player_Input {
	bool pressing_W;
	bool pressing_A;
	bool pressing_S;
	bool pressing_D;
	bool pressing_F;
	bool pressing_space;
};

class j1Player : public j1Entity {
public:
	j1Player();
	virtual ~j1Player();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void OnCollision(Collider* c1, Collider* c2);

	void MovementControl(float dt);

	bool Save(pugi::xml_node& data) const;
	bool Load(pugi::xml_node& data);

	//bool LoadAnimations();
	
public:
	float	jumpImpulse;
	float	doubleJumpImpulse;
	float	max_running_speed;
	float	acceleration;
	float	deceleration;
	float	max_side_speed;
	int		enemy_bouncing;

	bool	can_double_jump = true;
	bool	can_go_right = true;
	bool	can_go_left = true;

	EntityState last_state;

	p2SString	 folder;
	Player_Input player_input;

	pugi::xml_document animation_doc;

	//fx
	uint		jump_fx;
	uint		double_Jump_fx = 4;
	p2SString	jump_fx_path;
	p2SString	double_Jump_fx_path;

	bool god = false;
	bool controls_blocked = false;
	bool isVisible = true;

	Collider* last_checkpoint = nullptr;
};

#endif // !_j1PLAYER_H_

