#ifndef _GUIINPUTTEXT_H_
#define _GUIINPUTTEXT_H
#include "GuiImage.h"
#include "GuiText.h"
#include "j1UI_Element.h"

class GuiInputText : public j1UI_Element
{
public:
	GuiInputText(j1Module* callback);
	~GuiInputText();

	void InitializeInputText(iPoint position, p2SString text, SDL_Rect image_section);
	bool Input();
	bool Draw();

private:
	GuiImage* background;
	GuiText* text;
};


#endif // !_GUIINPUTTEXT_H_
