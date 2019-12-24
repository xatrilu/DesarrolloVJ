#include "j1App.h"
#include "j1Gui.h"
#include "GuiImage.h"
#include "j1Render.h"

GuiImage::GuiImage(j1Module* g_callback,bool Static)
{
	tex = (SDL_Texture*)App->gui->GetAtlas();
	callback = g_callback;
	isStatic = Static;
}


void GuiImage::Init(iPoint pos, SDL_Rect g_section) 
{
	screen_pos = pos;
	section = g_section;
	rect.x = screen_pos.x;
	rect.y = screen_pos.y;
	rect.w = section.w;
	rect.h = section.h;

	if (parent != nullptr)
	{
		local_pos.x = screen_pos.x - parent->screen_pos.x;
		local_pos.y = screen_pos.y - parent->screen_pos.y;
	}
}


bool GuiImage::Update(float dt) 
{
	bool ret = true;
	if (parent != nullptr)
	{
		screen_pos.x = parent->screen_pos.x + local_pos.x;
		screen_pos.y = parent->screen_pos.y + local_pos.y;
	}
	rect.x = screen_pos.x;
	rect.y = screen_pos.y;
	return ret;
}

void GuiImage::EmptyInit(SDL_Rect empty_rect) 
{
	screen_pos.x = rect.x = empty_rect.x;
	screen_pos.y = rect.y = empty_rect.y;
	rect.w = empty_rect.w;
	rect.h = empty_rect.h;

	if (parent != nullptr)
	{
		local_pos.x = screen_pos.x - parent->screen_pos.x;
		local_pos.y = screen_pos.y - parent->screen_pos.y;
	}
}

bool GuiImage::Draw()
{
	App->render->Blit(tex, rect.x, rect.y, &section);
	return true;
}

bool GuiImage::CleanUp() 
{
	bool ret = true;
	return ret;
}
