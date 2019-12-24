#ifndef _GUIINPUTTEXT_H_
#define _GUIINPUTTEXT_H
#include "GuiImage.h"
#include "GuiText.h"
#include "j1UI_Element.h"

class GuiInputText : public j1UI_Element
{
public:
	GuiInputText(j1Module* callback, bool Static);
	~GuiInputText();

	void Init(iPoint pos, p2SString text, SDL_Rect image_section, bool useAtlas = true);
	bool Update(float dt);
	bool Input();
	bool Draw();
	void HandleFocusEvent(FocusEvent event);
	GuiText* GetText() const;

private:
	GuiImage* background;
	p2SString default_text;
	GuiText* text;
	SDL_Rect mouse;
	bool focused;
	bool usingAtlas;
public:
	int mouse_pos;
};

#endif 
