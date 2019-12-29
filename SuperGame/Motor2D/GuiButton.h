#ifndef _GUIBUTTON_H_
#define _GUIBUTTON_H_
#include "j1UI_Element.h"
#include "SDL_image/include/SDL_image.h"

class p2SString;
class GuiText;

enum class ButtonAction {
	PLAY,
	CONTINUE,
	SETTINGS,
	CREDITS,
	QUIT,
	BACK,
	RESTART,
	ADRIA,
	XAVIER,
	FULLSCREEN,
	NONE
};

class GuiButton : public j1UI_Element
{
public:
	GuiButton(j1Module* callback);
	~GuiButton();

	void Init(iPoint position, SDL_Rect normalRect, SDL_Rect hoverRect, SDL_Rect clickRect, p2SString text, ButtonAction action = ButtonAction::NONE, bool cliking = false);
	bool Update(float dt);
	bool CleanUp();
	bool Input();
	bool Draw();

private:
	SDL_Texture*	texture;
	SDL_Rect*		currentRect;
	SDL_Rect		normalRect;
	SDL_Rect		hoverRect;
	SDL_Rect		clickRect;
	GuiText*		text;
public:
	ButtonAction	action;
	bool cliking;
};

#endif // !_GUIBUTTON_H_

