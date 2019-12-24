#ifndef _GUIIMAGE_H_
#define _GUIIMAGE_H_
#include "p2Point.h"
#include "j1UI_Element.h"
#include "SDL_image/include/SDL_image.h"

class GuiImage : public j1UI_Element
{
public:
	GuiImage();
	GuiImage(j1Module* callback,bool Static);
	~GuiImage() {}

	void Init(iPoint position, SDL_Rect section);
	void EmptyInit(SDL_Rect dimensions);
	bool Update(float dt);
	bool CleanUp();
	bool Draw();

public:
	SDL_Texture* tex;
	SDL_Rect section;
};


#endif // !_GUIIMAGE_H_
