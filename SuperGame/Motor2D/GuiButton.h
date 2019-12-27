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
	GO_BACK,
	RESTART,
	CONTEXTUAL_1,
	CONTEXTUAL_2,
	CONTEXTUAL_3,
	NONE
};

class GuiButton : public j1UI_Element
{
public:
	GuiButton(j1Module* callback);
	~GuiButton();

	void Init(iPoint position, SDL_Rect normal_rect, SDL_Rect hover_rect, SDL_Rect click_rect, p2SString text, ButtonAction action = ButtonAction::NONE, bool stay_clicked = false);
	bool Update(float dt);
	bool CleanUp();
	bool Input();
	bool Draw();

private:
	SDL_Texture* texture;
	SDL_Rect* current_rect;
	SDL_Rect		normal_rect;
	SDL_Rect		hover_rect;
	SDL_Rect		click_rect;
	GuiText* text;
public:
	ButtonAction	action;
	bool stay_clicked;
};

#endif // !_GUIBUTTON_H_

