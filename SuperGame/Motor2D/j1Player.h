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

	void OnCommand(p2SString command);

	//bool LoadAnimations();

public:
	float	jumpImpulse;
	float	doubleJumpImpulse;
	float	max_running_speed;
	float	acceleration;
	float	deceleration;
	float	max_side_speed;
	int		enemy_bouncing;
	int		lives;
	int		last_score;
	bool	can_double_jump;
	bool	can_go_right;
	bool	can_go_left;

	EntityState last_state;

	p2SString	 folder;
	Player_Input player_input;

	bool god;
	bool controls_blocked;
	bool isVisible;

	Collider* last_checkpoint = nullptr;
};

#endif // !_j1PLAYER_H_

