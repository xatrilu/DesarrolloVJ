#ifndef _GUIIMAGE_H_
#define _GUIIMAGE_H_
#include "p2Point.h"
#include "j1UI_Element.h"
#include "SDL_image/include/SDL_image.h"

class GuiImage : public j1UI_Element
{
public:
	GuiImage(j1Module* callback);
	~GuiImage() {}

	void InitializeImage(iPoint position, SDL_Rect section);
	bool Draw();

private:
	SDL_Texture* tex;
	SDL_Rect section;
};

#endif // !_GUIIMAGE_H_
