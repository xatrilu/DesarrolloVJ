#ifndef __j1FADETOBLACK_H__
#define __j1FADETOBLACK_H__

#include "j1Module.h"
#include "SDL\include\SDL_rect.h"

enum Map;

class j1FadeToBlack : public j1Module
{
public:
	j1FadeToBlack();
	~j1FadeToBlack();

	bool Start();
	//bool Update(float dt);
	bool PostUpdate();
	bool FadeToBlack(Map map_off, Map map_on, float time = 2.0f);

private:

	Map map_on;
	Map map_off;

	enum fade_step
	{
		none,
		fade_to_black,
		fade_from_black
	} current_step = fade_step::none;

	Uint32 start_time = 0;
	Uint32 total_time = 0;
	SDL_Rect screen;
};

#endif //__MODULEFADETOBLACK_H__
