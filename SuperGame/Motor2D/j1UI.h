#ifndef _j1UI_H
#define _j1UI_H

#include "j1Module.h"
#include "j1App.h"
#include "SDL/include/SDL_rect.h"

struct SDL_Texture;

enum TransitionState {
	STATIC,
	OPEN,
	CLOSE
};

#define TRANSITION_MARGIN 10

class j1UI : public j1Module
{
public:

	j1UI();
	virtual ~j1UI();

	bool Awake(pugi::xml_node& config);
	bool Start();


	bool Update(float dt);
	bool PostUpdate();

	bool CleanUp();

	void LevelTransition(float dt);
	void ResetTransition(TransitionState state = CLOSE);

public:
	//transition variables
	int transition_time = 3;
	int transition_moment = 0;
	float transition_speed = 0;

	TransitionState direction = CLOSE;
	SDL_Rect* camera;
	SDL_Rect left_square;
	SDL_Rect right_square;

	SDL_Texture* pause_tex = nullptr;

	bool loaded = false;
	bool transition = false;

	int deltaTime = 0;
	int time = 0;
};

#endif // !_j1UI_H

