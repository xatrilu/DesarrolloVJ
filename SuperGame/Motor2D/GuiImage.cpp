#include "j1App.h"
#include "j1Gui.h"
#include "GuiImage.h"
#include "j1Render.h"

GuiImage::GuiImage(j1Module* g_callback) {
	tex = (SDL_Texture*)App->gui->GetAtlas();
	callback = g_callback;
}

void GuiImage::InitializeImage(iPoint g_position, SDL_Rect g_section) {
	screen_position = g_position;
	section = g_section;
	rect.x = screen_position.x;
	rect.y = screen_position.y;
	rect.w = section.w;
	rect.h = section.h;

	if (parent != nullptr)
	{
		local_position.x = screen_position.x - parent->screen_position.x;
		local_position.y = screen_position.y - parent->screen_position.y;
	}
}

bool GuiImage::Draw() {
	App->render->Blit(tex, screen_position.x, screen_position.y, &section);
	return true;
}