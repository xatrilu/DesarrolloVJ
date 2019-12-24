#ifndef _GUIBUTTON_H_
#define _GUIBUTTON_H_
#include "j1UI_Element.h"
#include "SDL_image/include/SDL_image.h"

class p2SString;
class GuiText;

enum class ButtonAction
{
	NONE = 0,
	PLAY,
	CONTINUE,
	SETTINGS,
	CREDITS,
	QUIT,
	BACK,
	RESTART
};

class GuiButton : public j1UI_Element
{
public:
	GuiButton(j1Module* callback, bool isStatic);
	~GuiButton();

	void Init(iPoint position, SDL_Rect normal_rect, SDL_Rect hover_rect, SDL_Rect click_rect, p2SString text, ButtonAction action = ButtonAction::NONE);
	bool Update(float dt);
	bool CleanUp();
	bool Input();
	bool Draw();

private:
	SDL_Texture* tex;
	SDL_Rect  click_rect;
	SDL_Rect* current_rect;
	SDL_Rect  normal_rect;
	SDL_Rect  hover_rect;	
	GuiText*  text;
public:
	ButtonAction	action;
};

#endif 