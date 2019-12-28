#ifndef _GUIINPUTTEXT_H_
#define _GUIINPUTTEXT_H
#include "GuiImage.h"
#include "GuiText.h"
#include "j1UI_Element.h"

#define MAX_INPUT_CHARACTERS 50

class p2SString;

class GuiInputText : public j1UI_Element
{
public:
	GuiInputText(j1Module* callback);
	~GuiInputText();

	void Init(iPoint position, p2SString text, SDL_Rect image_section, bool useAtlas = true, char* font = DEFAULT_FONT);
	bool Update(float dt);
	bool Input();
	bool Draw();
	bool CleanUp();
	void HandleFocusEvent(FocusEvent event);
	void UpdateText();
	GuiText* GetText() const;

private:
	GuiImage* background;
	p2SString default_text;
	GuiText* text;
	SDL_Rect cursor;
	bool focused;
	bool usingAtlas;
public:
	int cursor_position;
};


#endif // !_GUIINPUTTEXT_H_

