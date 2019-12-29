#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "j1UI_Element.h"
#include "SDL/include/SDL.h"
#include "GuiButton.h"
#include "GuiImage.h"
#include "GuiText.h"
#include "GuiInputText.h"
//#include "GuiSlider.h"

class j1Button;
class j1StaticText;
class j1Image;
class j1Slider;

#define MOUSE_WIDTH 2

// ---------------------------------------------------
class j1Gui : public j1Module
{
public:

	j1Gui();
	virtual ~j1Gui();

	bool Awake(pugi::xml_node&);
	bool Start();

	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	bool CleanUp();

	virtual bool Input() { return true; };
	virtual bool Draw() { return true; };

	//Hover-Unnhover

	j1UI_Element* CreateUIElement(UI_Type type, j1Module* callback, j1UI_Element* parent = nullptr, bool draggable = false, bool interactable = false);
	void DestroyUIElement(j1UI_Element* element);
	void DestroyAllGui();
	void DestroyUIlist(p2List<j1UI_Element*> list);

	SDL_Texture* GetAtlas() const;

	void DebugDraw();

private:
	SDL_Texture* atlas;
	p2SString atlas_file_name;
	bool focused = false;

public:
	p2List<j1UI_Element*> UIelements;
	j1UI_Element* focused_element = nullptr;
	bool debug = false;
	uint	clickFX;
};

#endif // __j1GUI_H__