#ifndef _j1UIELEMENT_H_
#define _j1UIELEMENT_H_
#include "p2Point.h"

#include "SDL/include/SDL.h"

class j1Module;

enum class UI_Type
{
	BUTTON,
	IMAGE,
	TEXT,
	INPUT_TEXT,
	MAX_UI_ELEMENTS
};

class j1UI_Element
{
public:
	j1UI_Element() {};
	~j1UI_Element() {};

	virtual bool Input() { return true; }
	virtual bool Draw() { return true; }

	SDL_Rect GetScreenRect() const;
	SDL_Rect GetLocalRect() const;
	iPoint GetScreenPos() const;
	iPoint GetLocalPos() const;
	void SetLocalPos(iPoint new_position) { local_position = new_position; }
	bool MouseHovering();

public:
	iPoint			local_position;
	iPoint			screen_position;
	UI_Type			type;
	j1UI_Element* parent = nullptr;
	bool			draggable;
	j1Module* callback;

public:
	SDL_Rect rect;
};

#endif // !j1UIELEMENT_H

