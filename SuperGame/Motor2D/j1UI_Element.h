#ifndef _j1UIELEMENT_H_
#define _j1UIELEMENT_H_
#include "p2Point.h"

#include "SDL/include/SDL.h"

class j1Module;

enum class FocusEvent {
	FOCUS_IN,
	FOCUS_OUT,
	FOCUSED,
	CLICKED
};

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

	virtual void Init() {}
	virtual bool Input() { return true; }
	virtual bool Update(float dt) { return true; }
	virtual bool CleanUp() { return true; };

	virtual bool Draw() { return true; }
	virtual void HandleFocusEvent(FocusEvent event) {}

	SDL_Rect GetScreenRect() const;
	SDL_Rect GetLocalRect() const;
	iPoint GetScreenPos() const;
	iPoint GetLocalPos() const;
	void SetLocalPos(iPoint new_pos); 
	bool OnHover();

public:
	iPoint	localPos;
	iPoint	screenPos;
	UI_Type	type;
	FocusEvent focus_event;
	j1Module* callback = nullptr;
	j1UI_Element* parent = nullptr;
	SDL_Rect rect;
	SDL_Texture* texture = nullptr;

	bool draggable;
	bool interactable;
	bool isStatic;
	bool to_delete;	
};


#endif // !j1UIELEMENT_H

