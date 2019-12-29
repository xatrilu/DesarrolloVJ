#include "j1App.h"
#include "j1Gui.h"
#include "GuiImage.h"
#include "j1Render.h"
#include "j1Textures.h"

GuiImage::GuiImage(j1Module* callback) 
{
	texture = (SDL_Texture*)App->gui->GetAtlas();
	callback = callback;
	to_delete = false;
}

void GuiImage::Init(iPoint pos, SDL_Rect _section) 
{
	screenPos = pos;
	section = _section;
	rect.x = screenPos.x;
	rect.y = screenPos.y;
	rect.w = section.w;
	rect.h = section.h;
	if (parent != nullptr)
	{
		localPos.x = screenPos.x - parent->screenPos.x;
		localPos.y = screenPos.y - parent->screenPos.y;
	}
}

bool GuiImage::Update(float dt)
{
	bool ret = true;
	if (parent != nullptr)
	{
		screenPos.x = parent->screenPos.x + localPos.x;
		screenPos.y = parent->screenPos.y + localPos.y;
	}
	rect.x = screenPos.x - App->render->camera.x;
	rect.y = screenPos.y - App->render->camera.y;
	return ret;
}

bool GuiImage::Draw() 
{
	App->render->Blit(texture, rect.x, rect.y, &section);
	return true;
}

bool GuiImage::CleanUp()
{
	bool ret = true;
	texture = nullptr;
	return ret;
}